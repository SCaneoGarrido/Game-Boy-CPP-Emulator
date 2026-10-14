#include "TestHarness.h"

// SUITE cbrot: prefijo CB - RLC r (CB 00-07), RRC r (CB 08-0F),
//              RL r (CB 10-17) y RR r (CB 18-1F)  ->  Z 0 0 C
// RLC/RRC: el bit que sale (7 / 0) entra por el otro extremo y va a C.
// RL/RR:   el C previo entra por el otro extremo y el bit que sale va a C.
// A diferencia de RLCA/RRCA/RLA/RRA, Z refleja el resultado.
// M-cycles: 2 con registro, 4 con (HL). PC avanza 2 (CB + opcode).
// Checklist: testing/cbrot/checklist_cbrot.txt
int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> code = {
      0x06, 0x85,       // 1.  LD B,0x85
      0xCB, 0x00,       // 2.  RLC B
      0x0E, 0x00,       // 3.  LD C,0x00
      0xCB, 0x01,       // 4.  RLC C
      0x16, 0x80,       // 5.  LD D,0x80
      0xCB, 0x02,       // 6.  RLC D
      0x1E, 0x01,       // 7.  LD E,0x01
      0xCB, 0x03,       // 8.  RLC E
      0x26, 0x40,       // 9.  LD H,0x40
      0xCB, 0x04,       // 10. RLC H
      0x2E, 0xFF,       // 11. LD L,0xFF
      0xCB, 0x05,       // 12. RLC L
      0x3E, 0x7F,       // 13. LD A,0x7F
      0xCB, 0x07,       // 14. RLC A
      0x21, 0x00, 0xC0, // 15. LD HL,0xC000
      0x36, 0x81,       // 16. LD (HL),0x81
      0xCB, 0x06,       // 17. RLC (HL)
      0x06, 0x80,       // 18. LD B,0x80
      0xCB, 0x10,       // 19. RL B
      0x0E, 0x00,       // 20. LD C,0x00
      0xCB, 0x11,       // 21. RL C
      0x16, 0x80,       // 22. LD D,0x80
      0xCB, 0x12,       // 23. RL D
      0x1E, 0x40,       // 24. LD E,0x40
      0xCB, 0x13,       // 25. RL E
      0x26, 0xFF,       // 26. LD H,0xFF
      0xCB, 0x14,       // 27. RL H
      0x2E, 0x00,       // 28. LD L,0x00
      0xCB, 0x15,       // 29. RL L
      0x3E, 0x81,       // 30. LD A,0x81
      0xCB, 0x17,       // 31. RL A
      0x21, 0x00, 0xC0, // 32. LD HL,0xC000
      0x36, 0x7F,       // 33. LD (HL),0x7F
      0xCB, 0x16,       // 34. RL (HL)
      0xCB, 0x16,       // 35. RL (HL)
      0x06, 0x01,       // 36. LD B,0x01
      0xCB, 0x08,       // 37. RRC B
      0x0E, 0x00,       // 38. LD C,0x00
      0xCB, 0x09,       // 39. RRC C
      0x16, 0x02,       // 40. LD D,0x02
      0xCB, 0x0A,       // 41. RRC D
      0x1E, 0xFF,       // 42. LD E,0xFF
      0xCB, 0x0B,       // 43. RRC E
      0x3E, 0x81,       // 44. LD A,0x81
      0xCB, 0x0F,       // 45. RRC A
      0x36, 0x03,       // 46. LD (HL),0x03
      0xCB, 0x0E,       // 47. RRC (HL)
      0x26, 0x10,       // 48. LD H,0x10
      0xCB, 0x0C,       // 49. RRC H
      0x2E, 0x20,       // 50. LD L,0x20
      0xCB, 0x0D,       // 51. RRC L
      0x06, 0x01,       // 52. LD B,0x01
      0xCB, 0x18,       // 53. RR B
      0x0E, 0x00,       // 54. LD C,0x00
      0xCB, 0x19,       // 55. RR C
      0x16, 0x81,       // 56. LD D,0x81
      0xCB, 0x1A,       // 57. RR D
      0x1E, 0x02,       // 58. LD E,0x02
      0xCB, 0x1B,       // 59. RR E
      0x3E, 0x01,       // 60. LD A,0x01
      0xCB, 0x1F,       // 61. RR A
      0x26, 0xF0,       // 62. LD H,0xF0
      0xCB, 0x1C,       // 63. RR H
      0x2E, 0x0F,       // 64. LD L,0x0F
      0xCB, 0x1D,       // 65. RR L
      0x21, 0x00, 0xC0, // 66. LD HL,0xC000
      0xCB, 0x1E,       // 67. RR (HL)
      0xCB, 0x1E,       // 68. RR (HL)
  };

  std::vector<Step> steps = {
      {"LD B,85", E().b(0x85).pc(0x0102)},
      {"RLC B     85 -> 0B C", E().b(0x0B).f(0x10).pc(0x0104).cycles(2)},
      {"LD C,00", E().c(0x00).pc(0x0106)},
      {"RLC C     00 -> 00 Z", E().c(0x00).f(0x80).pc(0x0108).cycles(2)},
      {"LD D,80", E().d(0x80).pc(0x010A)},
      {"RLC D     80 -> 01 C", E().d(0x01).f(0x10).pc(0x010C).cycles(2)},
      {"LD E,01", E().e(0x01).pc(0x010E)},
      {"RLC E     01 -> 02", E().e(0x02).f(0x00).pc(0x0110).cycles(2)},
      {"LD H,40", E().h(0x40).pc(0x0112)},
      {"RLC H     40 -> 80", E().h(0x80).f(0x00).pc(0x0114).cycles(2)},
      {"LD L,FF", E().l(0xFF).pc(0x0116)},
      {"RLC L     FF -> FF C", E().l(0xFF).f(0x10).pc(0x0118).cycles(2)},
      {"LD A,7F", E().a(0x7F).pc(0x011A)},
      {"RLC A     7F -> FE", E().a(0xFE).f(0x00).pc(0x011C).cycles(2)},
      {"LD HL,C000", E().hl(0xC000).pc(0x011F)},
      {"LD (HL),81", E().mem(0xC000, 0x81).pc(0x0121)},
      {"RLC (HL)  81 -> 03 C", E().mem(0xC000, 0x03).f(0x10).pc(0x0123).cycles(4)},
      {"LD B,80", E().b(0x80).pc(0x0125)},
      {"RL B      80,C=1 -> 01 C", E().b(0x01).f(0x10).pc(0x0127).cycles(2)},
      {"LD C,00", E().c(0x00).pc(0x0129)},
      {"RL C      00,C=1 -> 01", E().c(0x01).f(0x00).pc(0x012B).cycles(2)},
      {"LD D,80", E().d(0x80).pc(0x012D)},
      {"RL D      80,C=0 -> 00 Z C", E().d(0x00).f(0x90).pc(0x012F).cycles(2)},
      {"LD E,40", E().e(0x40).pc(0x0131)},
      {"RL E      40,C=1 -> 81", E().e(0x81).f(0x00).pc(0x0133).cycles(2)},
      {"LD H,FF", E().h(0xFF).pc(0x0135)},
      {"RL H      FF,C=0 -> FE C", E().h(0xFE).f(0x10).pc(0x0137).cycles(2)},
      {"LD L,00", E().l(0x00).pc(0x0139)},
      {"RL L      00,C=1 -> 01", E().l(0x01).f(0x00).pc(0x013B).cycles(2)},
      {"LD A,81", E().a(0x81).pc(0x013D)},
      {"RL A      81,C=0 -> 02 C", E().a(0x02).f(0x10).pc(0x013F).cycles(2)},
      {"LD HL,C000", E().hl(0xC000).pc(0x0142)},
      {"LD (HL),7F", E().mem(0xC000, 0x7F).pc(0x0144)},
      {"RL (HL)   7F,C=1 -> FF", E().mem(0xC000, 0xFF).f(0x00).pc(0x0146).cycles(4)},
      {"RL (HL)   FF,C=0 -> FE C", E().mem(0xC000, 0xFE).f(0x10).pc(0x0148).cycles(4)},
      {"LD B,01", E().b(0x01).pc(0x014A)},
      {"RRC B     01 -> 80 C", E().b(0x80).f(0x10).pc(0x014C).cycles(2)},
      {"LD C,00", E().c(0x00).pc(0x014E)},
      {"RRC C     00 -> 00 Z", E().c(0x00).f(0x80).pc(0x0150).cycles(2)},
      {"LD D,02", E().d(0x02).pc(0x0152)},
      {"RRC D     02 -> 01", E().d(0x01).f(0x00).pc(0x0154).cycles(2)},
      {"LD E,FF", E().e(0xFF).pc(0x0156)},
      {"RRC E     FF -> FF C", E().e(0xFF).f(0x10).pc(0x0158).cycles(2)},
      {"LD A,81", E().a(0x81).pc(0x015A)},
      {"RRC A     81 -> C0 C", E().a(0xC0).f(0x10).pc(0x015C).cycles(2)},
      {"LD (HL),03", E().mem(0xC000, 0x03).pc(0x015E)},
      {"RRC (HL)  03 -> 81 C", E().mem(0xC000, 0x81).f(0x10).pc(0x0160).cycles(4)},
      {"LD H,10", E().h(0x10).pc(0x0162)},
      {"RRC H     10 -> 08", E().h(0x08).f(0x00).pc(0x0164).cycles(2)},
      {"LD L,20", E().l(0x20).pc(0x0166)},
      {"RRC L     20 -> 10", E().l(0x10).f(0x00).pc(0x0168).cycles(2)},
      {"LD B,01", E().b(0x01).pc(0x016A)},
      {"RR B      01,C=0 -> 00 Z C", E().b(0x00).f(0x90).pc(0x016C).cycles(2)},
      {"LD C,00", E().c(0x00).pc(0x016E)},
      {"RR C      00,C=1 -> 80", E().c(0x80).f(0x00).pc(0x0170).cycles(2)},
      {"LD D,81", E().d(0x81).pc(0x0172)},
      {"RR D      81,C=0 -> 40 C", E().d(0x40).f(0x10).pc(0x0174).cycles(2)},
      {"LD E,02", E().e(0x02).pc(0x0176)},
      {"RR E      02,C=1 -> 81", E().e(0x81).f(0x00).pc(0x0178).cycles(2)},
      {"LD A,01", E().a(0x01).pc(0x017A)},
      {"RR A      01,C=0 -> 00 Z C", E().a(0x00).f(0x90).pc(0x017C).cycles(2)},
      {"LD H,F0", E().h(0xF0).pc(0x017E)},
      {"RR H      F0,C=1 -> F8", E().h(0xF8).f(0x00).pc(0x0180).cycles(2)},
      {"LD L,0F", E().l(0x0F).pc(0x0182)},
      {"RR L      0F,C=0 -> 07 C", E().l(0x07).f(0x10).pc(0x0184).cycles(2)},
      {"LD HL,C000", E().hl(0xC000).pc(0x0187)},
      {"RR (HL)   81,C=1 -> C0 C", E().mem(0xC000, 0xC0).f(0x10).pc(0x0189).cycles(4)},
      {"RR (HL)   C0,C=1 -> E0", E().mem(0xC000, 0xE0).f(0x00).pc(0x018B).cycles(4)},
  };
  return runSuiteMain(argc, argv, "cbrot", code, {}, steps);
}
