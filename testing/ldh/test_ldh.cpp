#include "TestHarness.h"

// SUITE ldh: LDH (a8),A / LDH A,(a8) (E0 F0), LD (C),A / LD A,(C) (E2 F2),
//            LD (a16),A / LD A,(a16) (EA FA). No modifican flags.
// Usa HRAM (FF80-FFFE), WRAM (C100) y el espejo echo (E100 -> C100).
// Checklist: testing/ldh/checklist_ldh.txt
int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> code = {
      0x3E, 0x42,       // 1.  LD A,0x42
      0xE0, 0x80,       // 2.  LDH (0x80),A
      0x3E, 0x00,       // 3.  LD A,0x00
      0xF0, 0x80,       // 4.  LDH A,(0x80)
      0x0E, 0x81,       // 5.  LD C,0x81
      0x3E, 0x99,       // 6.  LD A,0x99
      0xE2,             // 7.  LD (C),A
      0x3E, 0x00,       // 8.  LD A,0x00
      0xF2,             // 9.  LD A,(C)
      0xEA, 0x00, 0xC1, // 10. LD (0xC100),A
      0x3E, 0x00,       // 11. LD A,0x00
      0xFA, 0x00, 0xC1, // 12. LD A,(0xC100)
      0xFA, 0x80, 0x01, // 13. LD A,(0x0180)  (ROM)
      0x3E, 0x55,       // 14. LD A,0x55
      0xEA, 0x00, 0xE1, // 15. LD (0xE100),A  (echo de C100)
      0x3E, 0x00,       // 16. LD A,0x00
      0xFA, 0x00, 0xC1, // 17. LD A,(0xC100)
  };
  std::vector<std::uint8_t> data = {0x77}; // 0x0180

  std::vector<Step> steps = {
      {"LD A,42", E().a(0x42).pc(0x0102)},
      {"LDH (80),A", E().mem(0xFF80, 0x42).pc(0x0104).f(0x00)},
      {"LD A,00", E().a(0x00).pc(0x0106)},
      {"LDH A,(80)", E().a(0x42).pc(0x0108).f(0x00)},
      {"LD C,81", E().c(0x81).pc(0x010A)},
      {"LD A,99", E().a(0x99).pc(0x010C)},
      {"LD (C),A  FF81", E().mem(0xFF81, 0x99).pc(0x010D).f(0x00)},
      {"LD A,00", E().a(0x00).pc(0x010F)},
      {"LD A,(C)  FF81", E().a(0x99).pc(0x0110).f(0x00)},
      {"LD (C100),A", E().mem(0xC100, 0x99).pc(0x0113).f(0x00)},
      {"LD A,00", E().a(0x00).pc(0x0115)},
      {"LD A,(C100)", E().a(0x99).pc(0x0118).f(0x00)},
      {"LD A,(0180) ROM", E().a(0x77).pc(0x011B).f(0x00)},
      {"LD A,55", E().a(0x55).pc(0x011D)},
      {"LD (E100),A echo", E().mem(0xE100, 0x55).mem(0xC100, 0x55).pc(0x0120)},
      {"LD A,00", E().a(0x00).pc(0x0122)},
      {"LD A,(C100) via echo", E().a(0x55).pc(0x0125).f(0x00)},
  };
  return runSuiteMain(argc, argv, "ldh", code, data, steps);
}
