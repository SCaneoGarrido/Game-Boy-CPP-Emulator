#include "TestHarness.h"

// SUITE rotates: RLCA (07), RRCA (0F), RLA (17), RRA (1F)  ->  0 0 0 C
// RLCA/RRCA: el bit que sale entra por el otro extremo y va a C.
// RLA/RRA:   el C previo entra por el otro extremo y el bit que sale va a C.
// Z SIEMPRE queda a 0, aunque A sea 0 (a diferencia de las versiones CB).
// Todas cuestan 1 M-cycle.
// Checklist: testing/rotates/checklist_rotates.txt
int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> code = {
      0x3E, 0x85, // 1.  LD A,0x85
      0x07,       // 2.  RLCA
      0x07,       // 3.  RLCA
      0x0F,       // 4.  RRCA
      0x0F,       // 5.  RRCA
      0x17,       // 6.  RLA
      0x17,       // 7.  RLA
      0x1F,       // 8.  RRA
      0x1F,       // 9.  RRA
      0xAF,       // 10. XOR A
      0x07,       // 11. RLCA
      0xAF,       // 12. XOR A
      0x17,       // 13. RLA
      0xAF,       // 14. XOR A
      0x0F,       // 15. RRCA
      0xAF,       // 16. XOR A
      0x1F,       // 17. RRA
      0x3E, 0x01, // 18. LD A,0x01
      0x1F,       // 19. RRA
      0x3E, 0x80, // 20. LD A,0x80
      0x17,       // 21. RLA
      0x2F,       // 22. CPL
      0x07,       // 23. RLCA
  };

  std::vector<Step> steps = {
      {"LD A,85", E().a(0x85).pc(0x0102)},
      {"RLCA      85 -> 0B C", E().a(0x0B).f(0x10).pc(0x0103).cycles(1)},
      {"RLCA      0B -> 16", E().a(0x16).f(0x00).pc(0x0104).cycles(1)},
      {"RRCA      16 -> 0B", E().a(0x0B).f(0x00).pc(0x0105).cycles(1)},
      {"RRCA      0B -> 85 C", E().a(0x85).f(0x10).pc(0x0106).cycles(1)},
      {"RLA       85,C=1 -> 0B C", E().a(0x0B).f(0x10).pc(0x0107).cycles(1)},
      {"RLA       0B,C=1 -> 17", E().a(0x17).f(0x00).pc(0x0108).cycles(1)},
      {"RRA       17,C=0 -> 0B C", E().a(0x0B).f(0x10).pc(0x0109).cycles(1)},
      {"RRA       0B,C=1 -> 85 C", E().a(0x85).f(0x10).pc(0x010A).cycles(1)},
      {"XOR A", E().a(0x00).f(0x80).pc(0x010B)},
      {"RLCA      00 -> 00, Z=0", E().a(0x00).f(0x00).pc(0x010C)},
      {"XOR A", E().f(0x80).pc(0x010D)},
      {"RLA       00 -> 00, Z=0", E().a(0x00).f(0x00).pc(0x010E)},
      {"XOR A", E().f(0x80).pc(0x010F)},
      {"RRCA      00 -> 00, Z=0", E().a(0x00).f(0x00).pc(0x0110)},
      {"XOR A", E().f(0x80).pc(0x0111)},
      {"RRA       00 -> 00, Z=0", E().a(0x00).f(0x00).pc(0x0112)},
      {"LD A,01", E().a(0x01).pc(0x0114)},
      {"RRA       01,C=0 -> 00 C, Z=0", E().a(0x00).f(0x10).pc(0x0115)},
      {"LD A,80", E().a(0x80).pc(0x0117)},
      {"RLA       80,C=1 -> 01 C", E().a(0x01).f(0x10).pc(0x0118)},
      {"CPL       01 -> FE (N H a 1)", E().a(0xFE).f(0x70).pc(0x0119)},
      {"RLCA      FE -> FD C, limpia N H", E().a(0xFD).f(0x10).pc(0x011A)},
  };
  return runSuiteMain(argc, argv, "rotates", code, {}, steps);
}
