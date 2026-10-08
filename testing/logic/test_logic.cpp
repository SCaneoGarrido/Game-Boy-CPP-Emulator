#include "TestHarness.h"

// SUITE logic: OR n (B0-B7, F6) -> Z 0 0 0
//              XOR n (A8-AF, EE) -> Z 0 0 0
//              CP n (B8-BF, FE)  -> Z 1 H C  (A no cambia)
// Checklist: testing/logic/checklist_logic.txt
int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> code = {
      0x06, 0xF0, // 1.  LD B,0xF0
      0x78,       // 2.  LD A,B
      0x0E, 0x0F, // 3.  LD C,0x0F
      0xB1,       // 4.  OR C
      0xA9,       // 5.  XOR C
      0xA8,       // 6.  XOR B
      0xB7,       // 7.  OR A
      0xF6, 0x0F, // 8.  OR 0x0F
      0xEE, 0xFF, // 9.  XOR 0xFF
      0x26, 0x01, // 10. LD H,0x01
      0x2E, 0x80, // 11. LD L,0x80   (HL=0180)
      0xB6,       // 12. OR (HL)
      0xAE,       // 13. XOR (HL)
      0xFE, 0xA0, // 14. CP 0xA0
      0xB8,       // 15. CP B
      0xB9,       // 16. CP C
      0xBE,       // 17. CP (HL)
      0xBF,       // 18. CP A
      0xFE, 0xFF, // 19. CP 0xFF     (pone C=1)
      0xB0,       // 20. OR B        (C debe limpiarse)
      0xFE, 0xFF, // 21. CP 0xFF     (pone C=1)
      0xAF,       // 22. XOR A       (C debe limpiarse, Z=1)
  };
  std::vector<std::uint8_t> data = {0x5A}; // 0x0180

  std::vector<Step> steps = {
      {"LD B,F0", E().b(0xF0).pc(0x0102)},
      {"LD A,B", E().a(0xF0).pc(0x0103)},
      {"LD C,0F", E().c(0x0F).pc(0x0105)},
      {"OR C      F0|0F", E().a(0xFF).f(0x00).pc(0x0106)},
      {"XOR C     FF^0F", E().a(0xF0).f(0x00).pc(0x0107)},
      {"XOR B     F0^F0", E().a(0x00).f(0x80).pc(0x0108)},
      {"OR A      00|00", E().a(0x00).f(0x80).pc(0x0109)},
      {"OR 0F     00|0F", E().a(0x0F).f(0x00).pc(0x010B)},
      {"XOR FF    0F^FF", E().a(0xF0).f(0x00).pc(0x010D)},
      {"LD H,01", E().h(0x01).pc(0x010F)},
      {"LD L,80", E().hl(0x0180).pc(0x0111)},
      {"OR (HL)   F0|5A", E().a(0xFA).f(0x00).pc(0x0112)},
      {"XOR (HL)  FA^5A", E().a(0xA0).f(0x00).pc(0x0113)},
      {"CP A0     A0=A0", E().a(0xA0).f(0xC0).pc(0x0115)},
      {"CP B      A0<F0 borrow", E().a(0xA0).f(0x50).pc(0x0116)},
      {"CP C      A0-0F half-borrow", E().a(0xA0).f(0x60).pc(0x0117)},
      {"CP (HL)   A0-5A half-borrow", E().a(0xA0).f(0x60).pc(0x0118)},
      {"CP A", E().a(0xA0).f(0xC0).pc(0x0119)},
      {"CP FF     -> C=1", E().a(0xA0).f(0x70).pc(0x011B)},
      {"OR B      A0|F0 limpia C", E().a(0xF0).f(0x00).pc(0x011C)},
      {"CP FF     -> C=1", E().a(0xF0).f(0x70).pc(0x011E)},
      {"XOR A     limpia C, Z=1", E().a(0x00).f(0x80).pc(0x011F)},
  };
  return runSuiteMain(argc, argv, "logic", code, data, steps);
}
