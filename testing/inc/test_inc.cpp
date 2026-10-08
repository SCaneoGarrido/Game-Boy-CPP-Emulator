#include "TestHarness.h"

// SUITE inc: INC r8 (Z 0 H -), INC (HL), INC rr / INC SP (sin flags)
// Checklist: testing/inc/checklist_inc.txt
int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> code = {
      0x06, 0x0F, // 1.  LD B,0x0F
      0x04,       // 2.  INC B
      0x04,       // 3.  INC B
      0x0E, 0xFF, // 4.  LD C,0xFF
      0x0C,       // 5.  INC C
      0x16, 0x7F, // 6.  LD D,0x7F
      0x14,       // 7.  INC D
      0x1C,       // 8.  INC E
      0x3C,       // 9.  INC A
      0x06, 0x00, // 10. LD B,0x00
      0x0E, 0xFF, // 11. LD C,0xFF   (BC=00FF)
      0x2E, 0xFF, // 12. LD L,0xFF
      0x2C,       // 13. INC L
      0x03,       // 14. INC BC
      0x26, 0xFF, // 15. LD H,0xFF
      0x2E, 0xFF, // 16. LD L,0xFF   (HL=FFFF)
      0x23,       // 17. INC HL
      0x16, 0x12, // 18. LD D,0x12
      0x1E, 0x34, // 19. LD E,0x34   (DE=1234)
      0x13,       // 20. INC DE
      0x33,       // 21. INC SP
      0x24,       // 22. INC H
      0x2E, 0x80, // 23. LD L,0x80   (HL=0180)
      0x34,       // 24. INC (HL)    (ROM: lee 0F, la escritura se descarta)
      0xD6, 0x02, // 25. SUB 0x02    (pone C=1)
      0x3C,       // 26. INC A       (C debe conservarse)
  };
  std::vector<std::uint8_t> data = {0x0F}; // 0x0180

  std::vector<Step> steps = {
      {"LD B,0F", E().b(0x0F).pc(0x0102)},
      {"INC B  0F->10 half-carry", E().b(0x10).f(0x20).pc(0x0103)},
      {"INC B  10->11 limpia H", E().b(0x11).f(0x00).pc(0x0104)},
      {"LD C,FF", E().c(0xFF).pc(0x0106)},
      {"INC C  FF->00 desborde", E().c(0x00).f(0xA0).pc(0x0107)},
      {"LD D,7F", E().d(0x7F).pc(0x0109)},
      {"INC D  7F->80 limpia Z", E().d(0x80).f(0x20).pc(0x010A)},
      {"INC E", E().e(0x01).f(0x00).pc(0x010B)},
      {"INC A", E().a(0x01).f(0x00).pc(0x010C)},
      {"LD B,00", E().bc(0x0000).pc(0x010E)},
      {"LD C,FF", E().bc(0x00FF).pc(0x0110)},
      {"LD L,FF", E().l(0xFF).pc(0x0112)},
      {"INC L  FF->00", E().l(0x00).f(0xA0).pc(0x0113)},
      {"INC BC 00FF->0100 sin flags", E().bc(0x0100).f(0xA0).pc(0x0114)},
      {"LD H,FF", E().h(0xFF).pc(0x0116)},
      {"LD L,FF", E().hl(0xFFFF).pc(0x0118)},
      {"INC HL FFFF->0000 sin flags", E().hl(0x0000).f(0xA0).pc(0x0119)},
      {"LD D,12", E().d(0x12).pc(0x011B)},
      {"LD E,34", E().de(0x1234).pc(0x011D)},
      {"INC DE", E().de(0x1235).f(0xA0).pc(0x011E)},
      {"INC SP", E().sp(0x0001).f(0xA0).pc(0x011F)},
      {"INC H  00->01", E().h(0x01).f(0x00).pc(0x0120)},
      {"LD L,80", E().hl(0x0180).pc(0x0122)},
      {"INC (HL) 0F->10, ROM protegida", E().f(0x20).mem(0x0180, 0x0F).pc(0x0123)},
      {"SUB 02  01-02 -> C=1", E().a(0xFF).f(0x70).pc(0x0125)},
      {"INC A  FF->00 conserva C", E().a(0x00).f(0xB0).pc(0x0126)},
  };
  return runSuiteMain(argc, argv, "inc", code, data, steps);
}
