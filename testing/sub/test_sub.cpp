#include "TestHarness.h"

// SUITE sub: SUB n (90-97, D6) y SBC A,n (98-9F, DE)  ->  Z 1 H C
// Checklist: testing/sub/checklist_sub.txt
int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> code = {
      0x06, 0x3E, // 1.  LD B,0x3E
      0x78,       // 2.  LD A,B
      0x0E, 0x3E, // 3.  LD C,0x3E
      0x91,       // 4.  SUB C
      0x06, 0x10, // 5.  LD B,0x10
      0x78,       // 6.  LD A,B
      0x16, 0x01, // 7.  LD D,0x01
      0x92,       // 8.  SUB D
      0x1E, 0x20, // 9.  LD E,0x20
      0x93,       // 10. SUB E
      0x97,       // 11. SUB A
      0x26, 0x01, // 12. LD H,0x01
      0x2E, 0x80, // 13. LD L,0x80   (HL=0180)
      0x96,       // 14. SUB (HL)
      0xD6, 0x0F, // 15. SUB 0x0F
      0xD6, 0xF1, // 16. SUB 0xF1
      0x98,       // 17. SBC A,B
      0xD6, 0xF0, // 18. SUB 0xF0
      0x9F,       // 19. SBC A,A
      0xDE, 0x00, // 20. SBC A,0x00
      0x9E,       // 21. SBC A,(HL)
  };
  std::vector<std::uint8_t> data = {0x01}; // 0x0180

  std::vector<Step> steps = {
      {"LD B,3E", E().b(0x3E).pc(0x0102)},
      {"LD A,B", E().a(0x3E).pc(0x0103)},
      {"LD C,3E", E().c(0x3E).pc(0x0105)},
      {"SUB C     3E-3E", E().a(0x00).f(0xC0).pc(0x0106)},
      {"LD B,10", E().b(0x10).pc(0x0108)},
      {"LD A,B", E().a(0x10).pc(0x0109)},
      {"LD D,01", E().d(0x01).pc(0x010B)},
      {"SUB D     10-01", E().a(0x0F).f(0x60).pc(0x010C)},
      {"LD E,20", E().e(0x20).pc(0x010E)},
      {"SUB E     0F-20", E().a(0xEF).f(0x50).pc(0x010F)},
      {"SUB A", E().a(0x00).f(0xC0).pc(0x0110)},
      {"LD H,01", E().h(0x01).pc(0x0112)},
      {"LD L,80", E().hl(0x0180).pc(0x0114)},
      {"SUB (HL)  00-01", E().a(0xFF).f(0x70).pc(0x0115)},
      {"SUB 0F    FF-0F", E().a(0xF0).f(0x40).pc(0x0117)},
      {"SUB F1    F0-F1", E().a(0xFF).f(0x70).pc(0x0119)},
      {"SBC A,B   FF-10-1", E().a(0xEE).f(0x40).pc(0x011A)},
      {"SUB F0    EE-F0", E().a(0xFE).f(0x50).pc(0x011C)},
      {"SBC A,A   FE-FE-1", E().a(0xFF).f(0x70).pc(0x011D)},
      {"SBC A,00  FF-00-1", E().a(0xFE).f(0x40).pc(0x011F)},
      {"SBC A,(HL) FE-01-0", E().a(0xFD).f(0x40).pc(0x0120)},
  };
  return runSuiteMain(argc, argv, "sub", code, data, steps);
}
