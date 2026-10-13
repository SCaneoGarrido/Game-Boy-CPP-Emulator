#include "TestHarness.h"

// SUITE misc: DAA (27), CPL (2F), SCF (37), CCF (3F), NOP (00)
// DAA: ajuste BCD de A segun N, H y C  ->  Z - 0 C
// CPL: A = ~A                          ->  - 1 1 -
// SCF: C = 1                           ->  - 0 0 1
// CCF: C = !C                          ->  - 0 0 C
// Todas cuestan 1 M-cycle.
// Checklist: testing/misc/checklist_misc.txt
int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> code = {
      0x3E, 0x15, // 1.  LD A,0x15
      0x06, 0x27, // 2.  LD B,0x27
      0x80,       // 3.  ADD A,B
      0x27,       // 4.  DAA
      0x3E, 0x38, // 5.  LD A,0x38
      0x06, 0x49, // 6.  LD B,0x49
      0x80,       // 7.  ADD A,B
      0x27,       // 8.  DAA
      0x3E, 0x90, // 9.  LD A,0x90
      0x06, 0x90, // 10. LD B,0x90
      0x80,       // 11. ADD A,B
      0x27,       // 12. DAA
      0x3E, 0x50, // 13. LD A,0x50
      0x06, 0x50, // 14. LD B,0x50
      0x80,       // 15. ADD A,B
      0x27,       // 16. DAA
      0x3E, 0x42, // 17. LD A,0x42
      0x06, 0x15, // 18. LD B,0x15
      0x90,       // 19. SUB B
      0x27,       // 20. DAA
      0x3E, 0x15, // 21. LD A,0x15
      0x06, 0x27, // 22. LD B,0x27
      0x90,       // 23. SUB B
      0x27,       // 24. DAA
      0x3E, 0x35, // 25. LD A,0x35
      0x2F,       // 26. CPL
      0x37,       // 27. SCF
      0x3F,       // 28. CCF
      0x3F,       // 29. CCF
      0xAF,       // 30. XOR A
      0x37,       // 31. SCF
      0x3F,       // 32. CCF
      0x2F,       // 33. CPL
      0x3F,       // 34. CCF
      0x00,       // 35. NOP
  };

  std::vector<Step> steps = {
      {"LD A,15", E().a(0x15).pc(0x0102)},
      {"LD B,27", E().b(0x27).pc(0x0104)},
      {"ADD A,B   15+27", E().a(0x3C).f(0x00).pc(0x0105)},
      {"DAA       3C -> 42 (BCD 15+27)", E().a(0x42).f(0x00).pc(0x0106).cycles(1)},
      {"LD A,38", E().a(0x38).pc(0x0108)},
      {"LD B,49", E().b(0x49).pc(0x010A)},
      {"ADD A,B   38+49", E().a(0x81).f(0x20).pc(0x010B)},
      {"DAA       81,H -> 87", E().a(0x87).f(0x00).pc(0x010C).cycles(1)},
      {"LD A,90", E().a(0x90).pc(0x010E)},
      {"LD B,90", E().b(0x90).pc(0x0110)},
      {"ADD A,B   90+90", E().a(0x20).f(0x10).pc(0x0111)},
      {"DAA       20,C -> 80 C", E().a(0x80).f(0x10).pc(0x0112).cycles(1)},
      {"LD A,50", E().a(0x50).pc(0x0114)},
      {"LD B,50", E().b(0x50).pc(0x0116)},
      {"ADD A,B   50+50", E().a(0xA0).f(0x00).pc(0x0117)},
      {"DAA       A0 -> 00 Z C", E().a(0x00).f(0x90).pc(0x0118).cycles(1)},
      {"LD A,42", E().a(0x42).pc(0x011A)},
      {"LD B,15", E().b(0x15).pc(0x011C)},
      {"SUB B     42-15", E().a(0x2D).f(0x60).pc(0x011D)},
      {"DAA       2D,N H -> 27", E().a(0x27).f(0x40).pc(0x011E).cycles(1)},
      {"LD A,15", E().a(0x15).pc(0x0120)},
      {"LD B,27", E().b(0x27).pc(0x0122)},
      {"SUB B     15-27", E().a(0xEE).f(0x70).pc(0x0123)},
      {"DAA       EE,N H C -> 88 C", E().a(0x88).f(0x50).pc(0x0124).cycles(1)},
      {"LD A,35", E().a(0x35).pc(0x0126)},
      {"CPL       35 -> CA", E().a(0xCA).f(0x70).pc(0x0127).cycles(1)},
      {"SCF       limpia N H", E().f(0x10).pc(0x0128).cycles(1)},
      {"CCF       C 1 -> 0", E().f(0x00).pc(0x0129).cycles(1)},
      {"CCF       C 0 -> 1", E().f(0x10).pc(0x012A).cycles(1)},
      {"XOR A", E().a(0x00).f(0x80).pc(0x012B)},
      {"SCF       Z se conserva", E().f(0x90).pc(0x012C).cycles(1)},
      {"CCF       Z se conserva", E().f(0x80).pc(0x012D).cycles(1)},
      {"CPL       00 -> FF, Z se conserva", E().a(0xFF).f(0xE0).pc(0x012E).cycles(1)},
      {"CCF       limpia N H, C 0 -> 1", E().f(0x90).pc(0x012F).cycles(1)},
      {"NOP       no toca nada", E().a(0xFF).f(0x90).pc(0x0130).cycles(1)},
  };
  return runSuiteMain(argc, argv, "misc", code, {}, steps);
}
