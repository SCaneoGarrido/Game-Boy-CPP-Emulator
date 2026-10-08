#include "TestHarness.h"

// SUITE ldsp: LD (a16),SP (08)  -> sin flags, escribe SP en little endian
//             LD HL,SP+e8 (F8)  -> 0 0 H C  (H/C del byte bajo sin signo, SP no cambia)
// [A5] 0x08 y 0xF8 no estan registrados en load_LD16BITS_block: hoy falla en el paso 2.
// Checklist: testing/ldsp/checklist_ldsp.txt
int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> code = {
      0x31, 0xF8, 0xFF, // 1. LD SP,0xFFF8
      0x08, 0x00, 0xC0, // 2. LD (0xC000),SP
      0xF8, 0x08,       // 3. LD HL,SP+8
      0xF8, 0xFF,       // 4. LD HL,SP-1
      0x31, 0x00, 0x10, // 5. LD SP,0x1000
      0xF8, 0x80,       // 6. LD HL,SP-128
  };

  std::vector<Step> steps = {
      {"LD SP,FFF8", E().sp(0xFFF8).pc(0x0103)},
      {"LD (C000),SP", E().mem(0xC000, 0xF8).mem(0xC001, 0xFF).sp(0xFFF8).pc(0x0106)},
      {"LD HL,SP+8   FFF8+08", E().hl(0x0000).sp(0xFFF8).f(0x30).pc(0x0108)},
      {"LD HL,SP-1   FFF8+FF", E().hl(0xFFF7).sp(0xFFF8).f(0x30).pc(0x010A)},
      {"LD SP,1000", E().sp(0x1000).pc(0x010D)},
      {"LD HL,SP-128 1000+80", E().hl(0x0F80).sp(0x1000).f(0x00).pc(0x010F)},
  };
  return runSuiteMain(argc, argv, "ldsp", code, {}, steps);
}
