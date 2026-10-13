#include "TestHarness.h"

// SUITE swap: prefijo CB - SWAP r (CB 30-37)  ->  Z 0 0 0
// Intercambia los nibbles alto y bajo. N, H y C siempre a 0.
// M-cycles: 2 con registro, 4 con (HL). PC avanza 2 (CB + opcode).
// Checklist: testing/swap/checklist_swap.txt
int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> code = {
      0x06, 0xF1,       // 1.  LD B,0xF1
      0xCB, 0x30,       // 2.  SWAP B
      0x0E, 0x00,       // 3.  LD C,0x00
      0xCB, 0x31,       // 4.  SWAP C
      0x37,             // 5.  SCF
      0x16, 0xAB,       // 6.  LD D,0xAB
      0xCB, 0x32,       // 7.  SWAP D
      0x1E, 0x0F,       // 8.  LD E,0x0F
      0xCB, 0x33,       // 9.  SWAP E
      0x26, 0x12,       // 10. LD H,0x12
      0xCB, 0x34,       // 11. SWAP H
      0x2E, 0x34,       // 12. LD L,0x34
      0xCB, 0x35,       // 13. SWAP L
      0x3E, 0x5A,       // 14. LD A,0x5A
      0xCB, 0x37,       // 15. SWAP A
      0x21, 0x00, 0xC0, // 16. LD HL,0xC000
      0x36, 0xC3,       // 17. LD (HL),0xC3
      0xCB, 0x36,       // 18. SWAP (HL)
      0x36, 0x00,       // 19. LD (HL),0x00
      0xCB, 0x36,       // 20. SWAP (HL)
  };

  std::vector<Step> steps = {
      {"LD B,F1", E().b(0xF1).pc(0x0102)},
      {"SWAP B    F1 -> 1F", E().b(0x1F).f(0x00).pc(0x0104).cycles(2)},
      {"LD C,00", E().c(0x00).pc(0x0106)},
      {"SWAP C    00 -> 00 Z", E().c(0x00).f(0x80).pc(0x0108).cycles(2)},
      {"SCF", E().f(0x90).pc(0x0109)},
      {"LD D,AB", E().d(0xAB).pc(0x010B)},
      {"SWAP D    AB -> BA, limpia C", E().d(0xBA).f(0x00).pc(0x010D).cycles(2)},
      {"LD E,0F", E().e(0x0F).pc(0x010F)},
      {"SWAP E    0F -> F0", E().e(0xF0).f(0x00).pc(0x0111).cycles(2)},
      {"LD H,12", E().h(0x12).pc(0x0113)},
      {"SWAP H    12 -> 21", E().h(0x21).f(0x00).pc(0x0115).cycles(2)},
      {"LD L,34", E().l(0x34).pc(0x0117)},
      {"SWAP L    34 -> 43", E().l(0x43).f(0x00).pc(0x0119).cycles(2)},
      {"LD A,5A", E().a(0x5A).pc(0x011B)},
      {"SWAP A    5A -> A5", E().a(0xA5).f(0x00).pc(0x011D).cycles(2)},
      {"LD HL,C000", E().hl(0xC000).pc(0x0120)},
      {"LD (HL),C3", E().mem(0xC000, 0xC3).pc(0x0122)},
      {"SWAP (HL) C3 -> 3C", E().mem(0xC000, 0x3C).f(0x00).pc(0x0124).cycles(4)},
      {"LD (HL),00", E().mem(0xC000, 0x00).pc(0x0126)},
      {"SWAP (HL) 00 -> 00 Z", E().mem(0xC000, 0x00).f(0x80).pc(0x0128).cycles(4)},
  };
  return runSuiteMain(argc, argv, "swap", code, {}, steps);
}
