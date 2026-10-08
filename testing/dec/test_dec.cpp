#include "TestHarness.h"

// SUITE dec: DEC r8 (Z 1 H -), DEC (HL), DEC rr / DEC SP (sin flags)
// Checklist: testing/dec/checklist_dec.txt
int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> code = {
      0x06, 0x10, // 1.  LD B,0x10
      0x05,       // 2.  DEC B
      0x0E, 0x01, // 3.  LD C,0x01
      0x0D,       // 4.  DEC C
      0x15,       // 5.  DEC D
      0x1E, 0x05, // 6.  LD E,0x05
      0x1D,       // 7.  DEC E
      0x3D,       // 8.  DEC A
      0x25,       // 9.  DEC H
      0x2D,       // 10. DEC L
      0x0B,       // 11. DEC BC
      0x06, 0x00, // 12. LD B,0x00
      0x0E, 0x00, // 13. LD C,0x00
      0x0B,       // 14. DEC BC
      0x1B,       // 15. DEC DE
      0x2B,       // 16. DEC HL
      0x3B,       // 17. DEC SP
      0x26, 0x01, // 18. LD H,0x01
      0x2E, 0x80, // 19. LD L,0x80   (HL=0180)
      0x35,       // 20. DEC (HL)    (ROM: lee 01, la escritura se descarta)
      0xC6, 0x01, // 21. ADD A,0x01  (pone C=1)
      0x3D,       // 22. DEC A       (C debe conservarse)
  };
  std::vector<std::uint8_t> data = {0x01}; // 0x0180

  std::vector<Step> steps = {
      {"LD B,10", E().b(0x10).pc(0x0102)},
      {"DEC B  10->0F borrow bit 4", E().b(0x0F).f(0x60).pc(0x0103)},
      {"LD C,01", E().c(0x01).pc(0x0105)},
      {"DEC C  01->00", E().c(0x00).f(0xC0).pc(0x0106)},
      {"DEC D  00->FF", E().d(0xFF).f(0x60).pc(0x0107)},
      {"LD E,05", E().e(0x05).pc(0x0109)},
      {"DEC E  05->04", E().e(0x04).f(0x40).pc(0x010A)},
      {"DEC A  00->FF", E().a(0xFF).f(0x60).pc(0x010B)},
      {"DEC H  00->FF", E().h(0xFF).f(0x60).pc(0x010C)},
      {"DEC L  00->FF", E().l(0xFF).f(0x60).pc(0x010D)},
      {"DEC BC 0F00->0EFF sin flags", E().bc(0x0EFF).f(0x60).pc(0x010E)},
      {"LD B,00", E().bc(0x00FF).pc(0x0110)},
      {"LD C,00", E().bc(0x0000).pc(0x0112)},
      {"DEC BC 0000->FFFF", E().bc(0xFFFF).f(0x60).pc(0x0113)},
      {"DEC DE FF04->FF03", E().de(0xFF03).pc(0x0114)},
      {"DEC HL FFFF->FFFE", E().hl(0xFFFE).pc(0x0115)},
      {"DEC SP 0000->FFFF", E().sp(0xFFFF).f(0x60).pc(0x0116)},
      {"LD H,01", E().h(0x01).pc(0x0118)},
      {"LD L,80", E().hl(0x0180).pc(0x011A)},
      {"DEC (HL) 01->00, ROM protegida", E().f(0xC0).mem(0x0180, 0x01).pc(0x011B)},
      {"ADD A,01  FF+01 -> C=1", E().a(0x00).f(0xB0).pc(0x011D)},
      {"DEC A  00->FF conserva C", E().a(0xFF).f(0x70).pc(0x011E)},
  };
  return runSuiteMain(argc, argv, "dec", code, data, steps);
}
