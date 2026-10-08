#include "TestHarness.h"

// SUITE add: ADD A,n (80-87, C6) y ADC A,n (88-8F, CE)  ->  Z 0 H C
// Checklist: testing/add/checklist_add.txt
int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> code = {
      0x06, 0x0F, // 1.  LD B,0x0F
      0x78,       // 2.  LD A,B
      0x0E, 0x01, // 3.  LD C,0x01
      0x81,       // 4.  ADD A,C
      0x16, 0x20, // 5.  LD D,0x20
      0x82,       // 6.  ADD A,D
      0x06, 0xF0, // 7.  LD B,0xF0
      0x80,       // 8.  ADD A,B
      0x1E, 0xE0, // 9.  LD E,0xE0
      0x83,       // 10. ADD A,E
      0x26, 0x01, // 11. LD H,0x01
      0x2E, 0x80, // 12. LD L,0x80   (HL=0180)
      0x86,       // 13. ADD A,(HL)
      0xC6, 0x08, // 14. ADD A,0x08
      0x87,       // 15. ADD A,A
      0x84,       // 16. ADD A,H
      0x85,       // 17. ADD A,L
      0x8F,       // 18. ADC A,A
      0x06, 0xF4, // 19. LD B,0xF4
      0x88,       // 20. ADC A,B
      0x89,       // 21. ADC A,C
      0xCE, 0x0F, // 22. ADC A,0x0F
      0x8E,       // 23. ADC A,(HL)
  };
  std::vector<std::uint8_t> data = {0x3A}; // 0x0180

  std::vector<Step> steps = {
      {"LD B,0F", E().b(0x0F).pc(0x0102)},
      {"LD A,B", E().a(0x0F).pc(0x0103)},
      {"LD C,01", E().c(0x01).pc(0x0105)},
      {"ADD A,C   0F+01", E().a(0x10).f(0x20).pc(0x0106)},
      {"LD D,20", E().d(0x20).pc(0x0108)},
      {"ADD A,D   10+20", E().a(0x30).f(0x00).pc(0x0109)},
      {"LD B,F0", E().b(0xF0).pc(0x010B)},
      {"ADD A,B   30+F0", E().a(0x20).f(0x10).pc(0x010C)},
      {"LD E,E0", E().e(0xE0).pc(0x010E)},
      {"ADD A,E   20+E0", E().a(0x00).f(0x90).pc(0x010F)},
      {"LD H,01", E().h(0x01).pc(0x0111)},
      {"LD L,80", E().hl(0x0180).pc(0x0113)},
      {"ADD A,(HL) 00+3A", E().a(0x3A).f(0x00).pc(0x0114)},
      {"ADD A,08  3A+08", E().a(0x42).f(0x20).pc(0x0116)},
      {"ADD A,A   42+42", E().a(0x84).f(0x00).pc(0x0117)},
      {"ADD A,H   84+01", E().a(0x85).f(0x00).pc(0x0118)},
      {"ADD A,L   85+80", E().a(0x05).f(0x10).pc(0x0119)},
      {"ADC A,A   05+05+1", E().a(0x0B).f(0x00).pc(0x011A)},
      {"LD B,F4", E().b(0xF4).pc(0x011C)},
      {"ADC A,B   0B+F4+0", E().a(0xFF).f(0x00).pc(0x011D)},
      {"ADC A,C   FF+01+0", E().a(0x00).f(0xB0).pc(0x011E)},
      {"ADC A,0F  00+0F+1", E().a(0x10).f(0x20).pc(0x0120)},
      {"ADC A,(HL) 10+3A+0", E().a(0x4A).f(0x00).pc(0x0121)},
  };
  return runSuiteMain(argc, argv, "add", code, data, steps);
}
