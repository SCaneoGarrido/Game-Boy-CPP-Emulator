#include "TestHarness.h"

// SUITE interrupts: atencion de una interrupcion en CPU::cpuCycle()
// Con IME=1 e IE & IF != 0, al inicio del ciclo la CPU:
//   IME=0, PUSH PC, limpia el bit de IF atendido, PC = vector (V-Blank -> 0040).
//   Cuesta 5 M-cycles y no ejecuta ninguna instruccion en ese ciclo.
// IF arranca en E1 (V-Blank pendiente); el programa habilita IE=01 y hace EI.
// Checklist: testing/interrupts/checklist_interrupts.txt
int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> code = {
      0x31, 0xFE, 0xFF, // 1. LD SP,0xFFFE
      0x3E, 0x01,       // 2. LD A,0x01
      0xEA, 0xFF, 0xFF, // 3. LD (0xFFFF),A   IE = V-Blank
      0xFB,             // 4. EI
      0x00,             // 5. NOP            (IME=1 al terminar)
      0x00,             // 6. -> se atiende V-Blank antes de ejecutar este byte
                        // 7. NOP en 0040 (ROM a cero)
  };

  std::vector<Step> steps = {
      {"LD SP,FFFE", E().sp(0xFFFE).pc(0x0103)},
      {"LD A,01", E().a(0x01).pc(0x0105)},
      {"LD (FFFF),A  IE=01, IME=0 -> no se atiende", E().pc(0x0108).ime(false)},
      {"EI           IME aun 0", E().ime(false).pc(0x0109)},
      {"NOP          IME=1, aun sin atender", E().ime(true).pc(0x010A)},
      {"V-Blank      PUSH 010A, PC=0040, IF bit0=0",
       E().ime(false).pc(0x0040).sp(0xFFFC).mem(0xFFFD, 0x01).mem(0xFFFC, 0x0A)
           .mem(0xFF0F, 0xE0).cycles(5)},
      {"NOP en 0040  IME=0 -> no se vuelve a atender", E().ime(false).pc(0x0041).sp(0xFFFC)},
  };
  return runSuiteMain(argc, argv, "interrupts", code, {}, steps);
}
