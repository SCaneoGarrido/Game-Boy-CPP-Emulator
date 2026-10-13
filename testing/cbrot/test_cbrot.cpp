#include "TestHarness.h"

// SUITE cbrot: prefijo CB - RLC r (CB 00-07) y RL r (CB 10-17)  ->  Z 0 0 C
// RLC: el bit 7 entra por el bit 0 y va a C.
// RL:  el C previo entra por el bit 0 y el bit 7 va a C.
// A diferencia de RLCA/RLA, Z refleja el resultado.
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
  };
  return runSuiteMain(argc, argv, "cbrot", code, {}, steps);
}
