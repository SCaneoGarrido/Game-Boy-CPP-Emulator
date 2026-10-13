#include "TestHarness.h"

// SUITE control: DI (F3), EI (FB), NOP (00), HALT (76), STOP (10 00)
// EI activa IME DESPUES de la instruccion siguiente; un DI en medio lo anula.
// HALT con IME=0:
//   - sin interrupcion pendiente (IE & IF = 0) -> la CPU se detiene hasta que haya una.
//   - con interrupcion pendiente -> HALT bug: no se detiene y el siguiente byte se lee 2 veces.
// STOP consume 2 bytes (10 00) y deja la CPU parada.
// IF arranca en E1 (V-Blank pendiente): IE decide si hay interrupcion pendiente.
// Los pokes a IE (FFFF) simulan el hardware externo que todavia no existe.
// Checklist: testing/control/checklist_control.txt
int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> code = {
      0xF3,       // 1.  DI
      0xFB,       // 2.  EI
      0x00,       // 3.  NOP
      0xF3,       // 4.  DI
      0xFB,       // 5.  EI
      0xF3,       // 6.  DI
      0x00,       // 7.  NOP
      0x76,       // 8.  HALT          (IE=00: sin interrupcion pendiente)
                  // 9.  (CPU en HALT)
      0x3C,       // 10. INC A         (poke IE=01 -> despierta)
      0x76,       // 11. HALT          (IME=0 e IE&IF!=0 -> HALT bug)
      0x3C,       // 12. INC A         (se ejecuta 2 veces, PC no avanza la 1a)
                  // 13. INC A (repetido)
      0x10, 0x00, // 14. STOP          (poke IE=00)
                  // 15. (CPU en STOP)
  };

  std::vector<Step> steps = {
      {"DI", E().ime(false).pc(0x0101).cycles(1)},
      {"EI        IME aun 0 (retraso)", E().ime(false).pc(0x0102).cycles(1)},
      {"NOP       IME=1 tras la instruccion siguiente", E().ime(true).pc(0x0103).cycles(1)},
      {"DI        IME=0 inmediato", E().ime(false).pc(0x0104).cycles(1)},
      {"EI", E().ime(false).pc(0x0105)},
      {"DI        anula el EI pendiente", E().ime(false).pc(0x0106)},
      {"NOP       IME sigue a 0", E().ime(false).pc(0x0107)},
      {"HALT      sin interrupcion -> CPU detenida",
       E().halted(true).pc(0x0108).cycles(1)},
      {"(HALT)    espera, PC congelado", E().halted(true).a(0x00).pc(0x0108).cycles(1)},
      {"INC A     IE=01 despierta la CPU", E().halted(false).a(0x01).f(0x00).pc(0x0109),
       {{0xFFFF, 0x01}}},
      {"HALT      HALT bug (IME=0, pendiente)", E().halted(false).pc(0x010A).cycles(1)},
      {"INC A     1a lectura, PC no avanza", E().a(0x02).f(0x00).pc(0x010A)},
      {"INC A     2a lectura del mismo byte", E().a(0x03).f(0x00).pc(0x010B)},
      {"STOP      consume 10 00", E().stopped(true).pc(0x010D).cycles(1), {{0xFFFF, 0x00}}},
      {"(STOP)    espera, PC congelado", E().stopped(true).pc(0x010D).cycles(1)},
  };
  return runSuiteMain(argc, argv, "control", code, {}, steps);
}
