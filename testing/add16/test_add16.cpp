#include "TestHarness.h"

// SUITE add16: ADD HL,rr (09 19 29 39) -> - 0 H C  (Z intacto, H bit 11, C bit 15)
//              ADD SP,e8 (E8)          -> 0 0 H C  (H/C del byte bajo sin signo)
// [A1] load_ADD16BITS_block no se llama en CPU::loadOpcodes(): hoy falla en el paso 2.
// Checklist: testing/add16/checklist_add16.txt
int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> code = {
      0x21, 0x00, 0x80, // 1.  LD HL,0x8000
      0x29,             // 2.  ADD HL,HL
      0x21, 0xFF, 0x0F, // 3.  LD HL,0x0FFF
      0x01, 0x01, 0x00, // 4.  LD BC,0x0001
      0xAF,             // 5.  XOR A        (Z=1)
      0x09,             // 6.  ADD HL,BC
      0x11, 0x00, 0xF0, // 7.  LD DE,0xF000
      0x19,             // 8.  ADD HL,DE
      0x21, 0x34, 0x12, // 9.  LD HL,0x1234
      0x31, 0x21, 0x43, // 10. LD SP,0x4321
      0x39,             // 11. ADD HL,SP
      0x29,             // 12. ADD HL,HL
      0x21, 0xFF, 0xFF, // 13. LD HL,0xFFFF
      0x09,             // 14. ADD HL,BC
      0x31, 0xF8, 0xFF, // 15. LD SP,0xFFF8
      0xE8, 0x08,       // 16. ADD SP,+8
      0xE8, 0xFF,       // 17. ADD SP,-1
      0xE8, 0x01,       // 18. ADD SP,+1
      0x31, 0x00, 0x10, // 19. LD SP,0x1000
      0xE8, 0x80,       // 20. ADD SP,-128
      0xE8, 0x7F,       // 21. ADD SP,+127
      0x31, 0x0F, 0x00, // 22. LD SP,0x000F
      0xAF,             // 23. XOR A        (Z=1)
      0xE8, 0x01,       // 24. ADD SP,+1
  };

  std::vector<Step> steps = {
      {"LD HL,8000", E().hl(0x8000).pc(0x0103)},
      {"ADD HL,HL  8000+8000 (Z no se activa)", E().hl(0x0000).f(0x10).pc(0x0104)},
      {"LD HL,0FFF", E().hl(0x0FFF).pc(0x0107)},
      {"LD BC,0001", E().bc(0x0001).pc(0x010A)},
      {"XOR A", E().a(0x00).f(0x80).pc(0x010B)},
      {"ADD HL,BC  0FFF+0001 (Z intacto)", E().hl(0x1000).f(0xA0).pc(0x010C)},
      {"LD DE,F000", E().de(0xF000).pc(0x010F)},
      {"ADD HL,DE  1000+F000", E().hl(0x0000).f(0x90).pc(0x0110)},
      {"LD HL,1234", E().hl(0x1234).pc(0x0113)},
      {"LD SP,4321", E().sp(0x4321).pc(0x0116)},
      {"ADD HL,SP  1234+4321", E().hl(0x5555).f(0x80).pc(0x0117)},
      {"ADD HL,HL  5555+5555", E().hl(0xAAAA).f(0x80).pc(0x0118)},
      {"LD HL,FFFF", E().hl(0xFFFF).pc(0x011B)},
      {"ADD HL,BC  FFFF+0001 (H y C)", E().hl(0x0000).f(0xB0).pc(0x011C)},
      {"LD SP,FFF8", E().sp(0xFFF8).pc(0x011F)},
      {"ADD SP,+8  (Z se limpia)", E().sp(0x0000).f(0x30).pc(0x0121)},
      {"ADD SP,-1", E().sp(0xFFFF).f(0x00).pc(0x0123)},
      {"ADD SP,+1", E().sp(0x0000).f(0x30).pc(0x0125)},
      {"LD SP,1000", E().sp(0x1000).pc(0x0128)},
      {"ADD SP,-128", E().sp(0x0F80).f(0x00).pc(0x012A)},
      {"ADD SP,+127", E().sp(0x0FFF).f(0x00).pc(0x012C)},
      {"LD SP,000F", E().sp(0x000F).pc(0x012F)},
      {"XOR A", E().f(0x80).pc(0x0130)},
      {"ADD SP,+1  (Z 1->0, H sin C)", E().sp(0x0010).f(0x20).pc(0x0132)},
  };
  return runSuiteMain(argc, argv, "add16", code, {}, steps);
}
