#include "TestHarness.h"

// SUITE ld16: LD A,(BC)/(DE)/(HL+)/(HL-) y LD (BC)/(DE)/(HL+)/(HL-),A
// LD no modifica flags: F=00 en todos los pasos.
// Checklist: testing/ld16/checklist_ld16.txt
int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> code = {
      0x06, 0x01, // 1.  LD B,0x01
      0x0E, 0x80, // 2.  LD C,0x80   (BC=0180)
      0x16, 0x01, // 3.  LD D,0x01
      0x1E, 0x81, // 4.  LD E,0x81   (DE=0181)
      0x26, 0x01, // 5.  LD H,0x01
      0x2E, 0x82, // 6.  LD L,0x82   (HL=0182)
      0x0A,       // 7.  LD A,(BC)
      0x1A,       // 8.  LD A,(DE)
      0x2A,       // 9.  LD A,(HL+)
      0x2A,       // 10. LD A,(HL+)
      0x3A,       // 11. LD A,(HL-)
      0x3A,       // 12. LD A,(HL-)
      0x02,       // 13. LD (BC),A   (ROM: se descarta)
      0x12,       // 14. LD (DE),A
      0x22,       // 15. LD (HL+),A
      0x32,       // 16. LD (HL-),A
      0x2E, 0xFF, // 17. LD L,0xFF   (HL=01FF)
      0x2A,       // 18. LD A,(HL+)  (acarreo de L a H)
  };
  std::vector<std::uint8_t> data = {0x10, 0x20, 0x30, 0x40, 0x50}; // 0x0180..

  std::vector<Step> steps = {
      {"LD B,01", E().b(0x01).pc(0x0102).f(0x00)},
      {"LD C,80", E().bc(0x0180).pc(0x0104).f(0x00)},
      {"LD D,01", E().d(0x01).pc(0x0106).f(0x00)},
      {"LD E,81", E().de(0x0181).pc(0x0108).f(0x00)},
      {"LD H,01", E().h(0x01).pc(0x010A).f(0x00)},
      {"LD L,82", E().hl(0x0182).pc(0x010C).f(0x00)},
      {"LD A,(BC)", E().a(0x10).pc(0x010D).f(0x00)},
      {"LD A,(DE)", E().a(0x20).pc(0x010E).f(0x00)},
      {"LD A,(HL+)", E().a(0x30).hl(0x0183).pc(0x010F).f(0x00)},
      {"LD A,(HL+)", E().a(0x40).hl(0x0184).pc(0x0110).f(0x00)},
      {"LD A,(HL-)", E().a(0x50).hl(0x0183).pc(0x0111).f(0x00)},
      {"LD A,(HL-)", E().a(0x40).hl(0x0182).pc(0x0112).f(0x00)},
      {"LD (BC),A  ROM protegida", E().mem(0x0180, 0x10).pc(0x0113).f(0x00)},
      {"LD (DE),A  ROM protegida", E().mem(0x0181, 0x20).pc(0x0114).f(0x00)},
      {"LD (HL+),A", E().mem(0x0182, 0x30).hl(0x0183).pc(0x0115).f(0x00)},
      {"LD (HL-),A", E().mem(0x0183, 0x40).hl(0x0182).pc(0x0116).f(0x00)},
      {"LD L,FF", E().hl(0x01FF).pc(0x0118).f(0x00)},
      {"LD A,(HL+) acarreo L->H", E().a(0x00).hl(0x0200).pc(0x0119).f(0x00)},
  };
  return runSuiteMain(argc, argv, "ld16", code, data, steps);
}
