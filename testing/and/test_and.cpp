#include "TestHarness.h"

// SUITE and: AND n (A0-A7, E6)  ->  Z 0 1 0
// Checklist: testing/and/checklist_and.txt
int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> code = {
      0x06, 0xF0, // 1.  LD B,0xF0
      0x78,       // 2.  LD A,B
      0x0E, 0x0F, // 3.  LD C,0x0F
      0xA1,       // 4.  AND C
      0x06, 0xFF, // 5.  LD B,0xFF
      0x78,       // 6.  LD A,B
      0xA1,       // 7.  AND C
      0x26, 0x01, // 8.  LD H,0x01
      0x2E, 0x80, // 9.  LD L,0x80   (HL=0180)
      0xA6,       // 10. AND (HL)
      0xE6, 0x04, // 11. AND 0x04
      0xA7,       // 12. AND A
      0xA0,       // 13. AND B
      0xA2,       // 14. AND D
      0xD6, 0x01, // 15. SUB 0x01    (pone C=1)
      0xE6, 0x0F, // 16. AND 0x0F    (C debe limpiarse)
  };
  std::vector<std::uint8_t> data = {0x3C}; // 0x0180

  std::vector<Step> steps = {
      {"LD B,F0", E().b(0xF0).pc(0x0102)},
      {"LD A,B", E().a(0xF0).pc(0x0103)},
      {"LD C,0F", E().c(0x0F).pc(0x0105)},
      {"AND C     F0&0F", E().a(0x00).f(0xA0).pc(0x0106)},
      {"LD B,FF", E().b(0xFF).pc(0x0108)},
      {"LD A,B", E().a(0xFF).pc(0x0109)},
      {"AND C     FF&0F limpia Z", E().a(0x0F).f(0x20).pc(0x010A)},
      {"LD H,01", E().h(0x01).pc(0x010C)},
      {"LD L,80", E().hl(0x0180).pc(0x010E)},
      {"AND (HL)  0F&3C", E().a(0x0C).f(0x20).pc(0x010F)},
      {"AND 04    0C&04", E().a(0x04).f(0x20).pc(0x0111)},
      {"AND A", E().a(0x04).f(0x20).pc(0x0112)},
      {"AND B     04&FF", E().a(0x04).f(0x20).pc(0x0113)},
      {"AND D     04&00", E().a(0x00).f(0xA0).pc(0x0114)},
      {"SUB 01    00-01 -> C=1", E().a(0xFF).f(0x70).pc(0x0116)},
      {"AND 0F    limpia C", E().a(0x0F).f(0x20).pc(0x0118)},
  };
  return runSuiteMain(argc, argv, "and", code, data, steps);
}
