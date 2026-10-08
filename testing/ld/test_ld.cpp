#include "TestHarness.h"

// SUITE ld: NOP, LD r8,d8, LD r8,r8, LD r8,(HL), LD (HL),r8
// LD no modifica flags: F=00 en todos los pasos.
// Checklist: testing/ld/checklist_ld.txt
int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> code = {
      0x00,       // 1.  NOP
      0x06, 0x11, // 2.  LD B,0x11
      0x0E, 0x22, // 3.  LD C,0x22
      0x16, 0x33, // 4.  LD D,0x33
      0x1E, 0x44, // 5.  LD E,0x44
      0x26, 0x01, // 6.  LD H,0x01
      0x2E, 0x80, // 7.  LD L,0x80   (HL=0180)
      0x3E, 0x55, // 8.  LD A,0x55
      0x78,       // 9.  LD A,B
      0x41,       // 10. LD B,C
      0x7F,       // 11. LD A,A
      0x4A,       // 12. LD C,D
      0x53,       // 13. LD D,E
      0x5F,       // 14. LD E,A
      0x7E,       // 15. LD A,(HL)
      0x46,       // 16. LD B,(HL)
      0x70,       // 17. LD (HL),B   (0180 es ROM: la escritura se descarta)
      0x77,       // 18. LD (HL),A
      0x6E,       // 19. LD L,(HL)
  };
  std::vector<std::uint8_t> data = {0xA5}; // 0x0180

  std::vector<Step> steps = {
      {"NOP", E().pc(0x0101).f(0x00)},
      {"LD B,11", E().b(0x11).pc(0x0103).f(0x00)},
      {"LD C,22", E().c(0x22).pc(0x0105).f(0x00)},
      {"LD D,33", E().d(0x33).pc(0x0107).f(0x00)},
      {"LD E,44", E().e(0x44).pc(0x0109).f(0x00)},
      {"LD H,01", E().h(0x01).pc(0x010B).f(0x00)},
      {"LD L,80", E().hl(0x0180).pc(0x010D).f(0x00)},
      {"LD A,55", E().a(0x55).hl(0x0180).pc(0x010F).f(0x00)},
      {"LD A,B", E().a(0x11).pc(0x0110).f(0x00)},
      {"LD B,C", E().b(0x22).pc(0x0111).f(0x00)},
      {"LD A,A", E().a(0x11).pc(0x0112).f(0x00)},
      {"LD C,D", E().c(0x33).pc(0x0113).f(0x00)},
      {"LD D,E", E().d(0x44).pc(0x0114).f(0x00)},
      {"LD E,A", E().e(0x11).pc(0x0115).f(0x00)},
      {"LD A,(HL)  (0180)=A5", E().a(0xA5).pc(0x0116).f(0x00)},
      {"LD B,(HL)", E().b(0xA5).pc(0x0117).f(0x00)},
      {"LD (HL),B  ROM protegida", E().mem(0x0180, 0xA5).pc(0x0118).f(0x00)},
      {"LD (HL),A  ROM protegida", E().mem(0x0180, 0xA5).pc(0x0119).f(0x00)},
      {"LD L,(HL)", E().hl(0x01A5).pc(0x011A).f(0x00)},
  };
  return runSuiteMain(argc, argv, "ld", code, data, steps);
}
