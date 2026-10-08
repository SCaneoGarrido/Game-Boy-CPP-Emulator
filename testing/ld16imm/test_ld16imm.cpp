#include "TestHarness.h"

// SUITE ld16imm: LD rr,d16 (01 11 21 31) y LD SP,HL (F9). No modifican flags.
// d16 va en little endian: byte bajo primero.
// Checklist: testing/ld16imm/checklist_ld16imm.txt
int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> code = {
      0x01, 0x34, 0x12, // 1. LD BC,0x1234
      0x11, 0x78, 0x56, // 2. LD DE,0x5678
      0x21, 0xBC, 0x9A, // 3. LD HL,0x9ABC
      0x31, 0xFE, 0xFF, // 4. LD SP,0xFFFE
      0xF9,             // 5. LD SP,HL
      0x21, 0x00, 0x00, // 6. LD HL,0x0000
      0xF9,             // 7. LD SP,HL
  };

  std::vector<Step> steps = {
      {"LD BC,1234", E().bc(0x1234).pc(0x0103).f(0x00)},
      {"LD DE,5678", E().de(0x5678).pc(0x0106).f(0x00)},
      {"LD HL,9ABC", E().hl(0x9ABC).pc(0x0109).f(0x00)},
      {"LD SP,FFFE", E().sp(0xFFFE).pc(0x010C).f(0x00)},
      {"LD SP,HL   9ABC", E().sp(0x9ABC).hl(0x9ABC).pc(0x010D).f(0x00)},
      {"LD HL,0000", E().hl(0x0000).pc(0x0110).f(0x00)},
      {"LD SP,HL   0000", E().sp(0x0000).pc(0x0111).f(0x00)},
  };
  return runSuiteMain(argc, argv, "ld16imm", code, {}, steps);
}
