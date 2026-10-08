#include "TestHarness.h"

// SUITE mem: persistencia en WRAM de las instrucciones que escriben en memoria:
// LD (HL),d8 (36), INC/DEC (HL), LD (HL),r / r,(HL), LD (HL+),A, ADD A,(HL),
// LD (BC),A / LD A,(BC).
// Checklist: testing/mem/checklist_mem.txt
int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> code = {
      0x21, 0x00, 0xC0, // 1.  LD HL,0xC000
      0x36, 0x0F,       // 2.  LD (HL),0x0F
      0x34,             // 3.  INC (HL)
      0x7E,             // 4.  LD A,(HL)
      0x35,             // 5.  DEC (HL)
      0x35,             // 6.  DEC (HL)
      0x06, 0xAB,       // 7.  LD B,0xAB
      0x70,             // 8.  LD (HL),B
      0x4E,             // 9.  LD C,(HL)
      0x22,             // 10. LD (HL+),A
      0x36, 0x01,       // 11. LD (HL),0x01
      0x35,             // 12. DEC (HL)
      0x2B,             // 13. DEC HL
      0x86,             // 14. ADD A,(HL)
      0x01, 0x00, 0xC1, // 15. LD BC,0xC100
      0x02,             // 16. LD (BC),A
      0x3E, 0x00,       // 17. LD A,0x00
      0x0A,             // 18. LD A,(BC)
  };

  std::vector<Step> steps = {
      {"LD HL,C000", E().hl(0xC000).pc(0x0103)},
      {"LD (HL),0F", E().mem(0xC000, 0x0F).pc(0x0105)},
      {"INC (HL)  0F->10", E().mem(0xC000, 0x10).f(0x20).pc(0x0106)},
      {"LD A,(HL)", E().a(0x10).pc(0x0107)},
      {"DEC (HL)  10->0F", E().mem(0xC000, 0x0F).f(0x60).pc(0x0108)},
      {"DEC (HL)  0F->0E", E().mem(0xC000, 0x0E).f(0x40).pc(0x0109)},
      {"LD B,AB", E().b(0xAB).pc(0x010B)},
      {"LD (HL),B", E().mem(0xC000, 0xAB).pc(0x010C)},
      {"LD C,(HL)", E().c(0xAB).pc(0x010D)},
      {"LD (HL+),A", E().mem(0xC000, 0x10).hl(0xC001).pc(0x010E)},
      {"LD (HL),01", E().mem(0xC001, 0x01).pc(0x0110)},
      {"DEC (HL)  01->00", E().mem(0xC001, 0x00).f(0xC0).pc(0x0111)},
      {"DEC HL", E().hl(0xC000).pc(0x0112)},
      {"ADD A,(HL) 10+10", E().a(0x20).f(0x00).pc(0x0113)},
      {"LD BC,C100", E().bc(0xC100).pc(0x0116)},
      {"LD (BC),A", E().mem(0xC100, 0x20).pc(0x0117)},
      {"LD A,00", E().a(0x00).pc(0x0119)},
      {"LD A,(BC)", E().a(0x20).pc(0x011A)},
  };
  return runSuiteMain(argc, argv, "mem", code, {}, steps);
}
