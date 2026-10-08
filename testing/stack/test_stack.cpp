#include "TestHarness.h"

// SUITE stack: PUSH rr (C5 D5 E5 F5) y POP rr (C1 D1 E1 F1)
// PUSH: SP-=2, (SP+1)=alto, (SP)=bajo. POP AF deja a 0 los 4 bits bajos de F.
// La pila vive en WRAM (SP=D000 -> usa CFFE/CFFF).
// Checklist: testing/stack/checklist_stack.txt
int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> code = {
      0x31, 0x00, 0xD0, // 1.  LD SP,0xD000
      0x01, 0x34, 0x12, // 2.  LD BC,0x1234
      0xC5,             // 3.  PUSH BC
      0x11, 0x78, 0x56, // 4.  LD DE,0x5678
      0xD5,             // 5.  PUSH DE
      0xE1,             // 6.  POP HL
      0xD1,             // 7.  POP DE
      0x06, 0xFF,       // 8.  LD B,0xFF
      0x0E, 0xFF,       // 9.  LD C,0xFF
      0xC5,             // 10. PUSH BC
      0xF1,             // 11. POP AF
      0xF5,             // 12. PUSH AF
      0xC1,             // 13. POP BC
      0x21, 0xCD, 0xAB, // 14. LD HL,0xABCD
      0xE5,             // 15. PUSH HL
      0xF1,             // 16. POP AF
  };

  std::vector<Step> steps = {
      {"LD SP,D000", E().sp(0xD000).pc(0x0103)},
      {"LD BC,1234", E().bc(0x1234).pc(0x0106)},
      {"PUSH BC", E().sp(0xCFFE).mem(0xCFFF, 0x12).mem(0xCFFE, 0x34).pc(0x0107)},
      {"LD DE,5678", E().de(0x5678).pc(0x010A)},
      {"PUSH DE", E().sp(0xCFFC).mem(0xCFFD, 0x56).mem(0xCFFC, 0x78).pc(0x010B)},
      {"POP HL", E().hl(0x5678).sp(0xCFFE).pc(0x010C)},
      {"POP DE", E().de(0x1234).sp(0xD000).pc(0x010D)},
      {"LD B,FF", E().b(0xFF).pc(0x010F)},
      {"LD C,FF", E().bc(0xFFFF).pc(0x0111)},
      {"PUSH BC", E().sp(0xCFFE).mem(0xCFFF, 0xFF).mem(0xCFFE, 0xFF).pc(0x0112)},
      {"POP AF    FFFF -> F=F0", E().a(0xFF).f(0xF0).sp(0xD000).pc(0x0113)},
      {"PUSH AF", E().sp(0xCFFE).mem(0xCFFF, 0xFF).mem(0xCFFE, 0xF0).pc(0x0114)},
      {"POP BC", E().bc(0xFFF0).sp(0xD000).pc(0x0115)},
      {"LD HL,ABCD", E().hl(0xABCD).pc(0x0118)},
      {"PUSH HL", E().sp(0xCFFE).mem(0xCFFF, 0xAB).mem(0xCFFE, 0xCD).pc(0x0119)},
      {"POP AF    ABCD -> F=C0", E().a(0xAB).f(0xC0).sp(0xD000).pc(0x011A)},
  };
  return runSuiteMain(argc, argv, "stack", code, {}, steps);
}
