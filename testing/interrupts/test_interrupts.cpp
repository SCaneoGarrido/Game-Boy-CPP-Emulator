#include "TestHarness.h"

// SUITE interrupts: peticion y atencion de interrupciones en CPU::cpuCycle()
// IF (FF0F) se escribe con LDH (0F),A; al leerlo los bits 5-7 valen 1.
// Con IME=1 e IE & IF != 0, al inicio del ciclo la CPU:
//   IME=0, PUSH PC, limpia el bit de IF atendido, PC = vector.
//   Prioridad: V-Blank (bit 0, 0040) > LCD (0048) > Timer (0050) > Serial (0058) > Joypad (0060).
//   Cuesta 5 M-cycles y no ejecuta ninguna instruccion en ese ciclo.
// IF arranca en E1 (V-Blank pendiente): el programa lo limpia antes de empezar.
// Checklist: testing/interrupts/checklist_interrupts.txt
int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> code = {
      0x31, 0xFE, 0xFF, // 1.  LD SP,0xFFFE
      0xAF,             // 2.  XOR A
      0xE0, 0x0F,       // 3.  LDH (0x0F),A   IF = 00
      0x3E, 0x1F,       // 4.  LD A,0x1F
      0xEA, 0xFF, 0xFF, // 5.  LD (0xFFFF),A  IE = todas
      0x3E, 0x05,       // 6.  LD A,0x05
      0xE0, 0x0F,       // 7.  LDH (0x0F),A   IF = Timer + V-Blank
      0xFB,             // 8.  EI
      0x00,             // 9.  NOP            (IME=1 al terminar)
      0x00,             // 10. -> se atiende V-Blank antes de ejecutar este byte
                        // 11. NOP en 0040 (ROM a cero)
  };

  std::vector<Step> steps = {
      {"LD SP,FFFE", E().sp(0xFFFE).pc(0x0103)},
      {"XOR A", E().a(0x00).f(0x80).pc(0x0104)},
      {"LDH (0F),A   IF=00", E().mem(0xFF0F, 0xE0).pc(0x0106)},
      {"LD A,1F", E().a(0x1F).pc(0x0108)},
      {"LD (FFFF),A  IE=1F, nada pendiente", E().ime(false).pc(0x010B)},
      {"LD A,05", E().a(0x05).pc(0x010D)},
      {"LDH (0F),A   IF=05, IME=0", E().mem(0xFF0F, 0xE5).ime(false).pc(0x010F)},
      {"EI           IME aun 0", E().ime(false).pc(0x0110)},
      {"NOP          IME=1, aun sin atender", E().ime(true).pc(0x0111)},
      {"V-Blank      prioridad sobre Timer",
       E().ime(false).pc(0x0040).sp(0xFFFC).mem(0xFFFD, 0x01).mem(0xFFFC, 0x11)
           .mem(0xFF0F, 0xE4).cycles(5)},
      {"NOP en 0040  IME=0 -> Timer sigue pendiente",
       E().ime(false).pc(0x0041).sp(0xFFFC).mem(0xFF0F, 0xE4)},
  };
  return runSuiteMain(argc, argv, "interrupts", code, {}, steps);
}
