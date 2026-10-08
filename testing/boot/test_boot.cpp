#include "TestHarness.h"

// SUITE boot: estado de la CPU tras saltar la boot ROM (CPU::initCpu, hallazgo B9).
// Valores post-boot del DMG: A=01 F=B0 (Z-HC) B=00 C=13 D=00 E=D8 H=01 L=4D
//                            SP=FFFE PC=0100
// Es la unica suite que arranca con Start::PostBoot; el resto fija todo a 0.
// Checklist: testing/boot/checklist_boot.txt
int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> code = {
      0x00, // 1. NOP      (no toca nada: deja ver el estado post-boot)
      0xF5, // 2. PUSH AF  (SP=FFFE apunta a HRAM: la pila es usable desde el arranque)
      0xC1, // 3. POP BC
  };

  std::vector<Step> steps = {
      {"NOP  estado post-boot",
       E().a(0x01).f(0xB0).bc(0x0013).de(0x00D8).hl(0x014D).sp(0xFFFE).pc(0x0101)},
      {"PUSH AF  pila en HRAM",
       E().sp(0xFFFC).mem(0xFFFD, 0x01).mem(0xFFFC, 0xB0).pc(0x0102)},
      {"POP BC", E().bc(0x01B0).sp(0xFFFE).pc(0x0103)},
  };
  return runSuiteMain(argc, argv, "boot", code, {}, steps, Start::PostBoot);
}
