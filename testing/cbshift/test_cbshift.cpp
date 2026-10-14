#include "TestHarness.h"

// SUITE cbshift: prefijo CB - SLA r (CB 20-27), SRA r (CB 28-2F), SRL r (CB 38-3F)
//                ->  Z 0 0 C
// SLA: desplaza a la izquierda, entra un 0 por el bit 0, el bit 7 va a C.
// SRA: desplaza a la derecha conservando el bit 7 (signo), el bit 0 va a C.
// SRL: desplaza a la derecha, entra un 0 por el bit 7, el bit 0 va a C.
// CPL (pasos 5 y 27) pone N y H a 1 para comprobar que se limpian.
// M-cycles: 2 con registro, 4 con (HL). PC avanza 2 (CB + opcode).
// Checklist: testing/cbshift/checklist_cbshift.txt
int main(int argc, char *argv[]) {
  std::vector<std::uint8_t> code = {
      0x06, 0x80,       // 1.  LD B,0x80
      0xCB, 0x20,       // 2.  SLA B
      0x0E, 0x41,       // 3.  LD C,0x41
      0xCB, 0x21,       // 4.  SLA C
      0x2F,             // 5.  CPL
      0x16, 0xC1,       // 6.  LD D,0xC1
      0xCB, 0x22,       // 7.  SLA D
      0x1E, 0xFF,       // 8.  LD E,0xFF
      0xCB, 0x23,       // 9.  SLA E
      0x26, 0x40,       // 10. LD H,0x40
      0xCB, 0x24,       // 11. SLA H
      0x2E, 0x08,       // 12. LD L,0x08
      0xCB, 0x25,       // 13. SLA L
      0x3E, 0x7F,       // 14. LD A,0x7F
      0xCB, 0x27,       // 15. SLA A
      0x21, 0x00, 0xC0, // 16. LD HL,0xC000
      0x36, 0xC0,       // 17. LD (HL),0xC0
      0xCB, 0x26,       // 18. SLA (HL)
      0x06, 0x81,       // 19. LD B,0x81
      0xCB, 0x28,       // 20. SRA B
      0x0E, 0x01,       // 21. LD C,0x01
      0xCB, 0x29,       // 22. SRA C
      0x16, 0x7E,       // 23. LD D,0x7E
      0xCB, 0x2A,       // 24. SRA D
      0x1E, 0xFF,       // 25. LD E,0xFF
      0xCB, 0x2B,       // 26. SRA E
      0x2F,             // 27. CPL
      0x26, 0x80,       // 28. LD H,0x80
      0xCB, 0x2C,       // 29. SRA H
      0x2E, 0x02,       // 30. LD L,0x02
      0xCB, 0x2D,       // 31. SRA L
      0x3E, 0x40,       // 32. LD A,0x40
      0xCB, 0x2F,       // 33. SRA A
      0x21, 0x00, 0xC0, // 34. LD HL,0xC000
      0x36, 0x85,       // 35. LD (HL),0x85
      0xCB, 0x2E,       // 36. SRA (HL)
      0x06, 0x81,       // 37. LD B,0x81
      0xCB, 0x38,       // 38. SRL B
      0x0E, 0x01,       // 39. LD C,0x01
      0xCB, 0x39,       // 40. SRL C
      0x16, 0xFF,       // 41. LD D,0xFF
      0xCB, 0x3A,       // 42. SRL D
      0x1E, 0x80,       // 43. LD E,0x80
      0xCB, 0x3B,       // 44. SRL E
      0x26, 0x02,       // 45. LD H,0x02
      0xCB, 0x3C,       // 46. SRL H
      0x2E, 0x10,       // 47. LD L,0x10
      0xCB, 0x3D,       // 48. SRL L
      0x3E, 0xF0,       // 49. LD A,0xF0
      0xCB, 0x3F,       // 50. SRL A
      0x21, 0x00, 0xC0, // 51. LD HL,0xC000
      0x36, 0x03,       // 52. LD (HL),0x03
      0xCB, 0x3E,       // 53. SRL (HL)
      0xCB, 0x3E,       // 54. SRL (HL)
  };

  std::vector<Step> steps = {
      {"LD B,80", E().b(0x80).pc(0x0102)},
      {"SLA B     80 -> 00 Z C", E().b(0x00).f(0x90).pc(0x0104).cycles(2)},
      {"LD C,41", E().c(0x41).pc(0x0106)},
      {"SLA C     41 -> 82", E().c(0x82).f(0x00).pc(0x0108).cycles(2)},
      {"CPL       A=FF, N H a 1", E().a(0xFF).f(0x60).pc(0x0109)},
      {"LD D,C1", E().d(0xC1).pc(0x010B)},
      {"SLA D     C1 -> 82 C, limpia N H", E().d(0x82).f(0x10).pc(0x010D).cycles(2)},
      {"LD E,FF", E().e(0xFF).pc(0x010F)},
      {"SLA E     FF -> FE C", E().e(0xFE).f(0x10).pc(0x0111).cycles(2)},
      {"LD H,40", E().h(0x40).pc(0x0113)},
      {"SLA H     40 -> 80", E().h(0x80).f(0x00).pc(0x0115).cycles(2)},
      {"LD L,08", E().l(0x08).pc(0x0117)},
      {"SLA L     08 -> 10", E().l(0x10).f(0x00).pc(0x0119).cycles(2)},
      {"LD A,7F", E().a(0x7F).pc(0x011B)},
      {"SLA A     7F -> FE", E().a(0xFE).f(0x00).pc(0x011D).cycles(2)},
      {"LD HL,C000", E().hl(0xC000).pc(0x0120)},
      {"LD (HL),C0", E().mem(0xC000, 0xC0).pc(0x0122)},
      {"SLA (HL)  C0 -> 80 C", E().mem(0xC000, 0x80).f(0x10).pc(0x0124).cycles(4)},
      {"LD B,81", E().b(0x81).pc(0x0126)},
      {"SRA B     81 -> C0 C (conserva bit 7)", E().b(0xC0).f(0x10).pc(0x0128).cycles(2)},
      {"LD C,01", E().c(0x01).pc(0x012A)},
      {"SRA C     01 -> 00 Z C", E().c(0x00).f(0x90).pc(0x012C).cycles(2)},
      {"LD D,7E", E().d(0x7E).pc(0x012E)},
      {"SRA D     7E -> 3F", E().d(0x3F).f(0x00).pc(0x0130).cycles(2)},
      {"LD E,FF", E().e(0xFF).pc(0x0132)},
      {"SRA E     FF -> FF C", E().e(0xFF).f(0x10).pc(0x0134).cycles(2)},
      {"CPL       A=01, N H a 1", E().a(0x01).f(0x70).pc(0x0135)},
      {"LD H,80", E().h(0x80).pc(0x0137)},
      {"SRA H     80 -> C0, limpia N H C", E().h(0xC0).f(0x00).pc(0x0139).cycles(2)},
      {"LD L,02", E().l(0x02).pc(0x013B)},
      {"SRA L     02 -> 01", E().l(0x01).f(0x00).pc(0x013D).cycles(2)},
      {"LD A,40", E().a(0x40).pc(0x013F)},
      {"SRA A     40 -> 20", E().a(0x20).f(0x00).pc(0x0141).cycles(2)},
      {"LD HL,C000", E().hl(0xC000).pc(0x0144)},
      {"LD (HL),85", E().mem(0xC000, 0x85).pc(0x0146)},
      {"SRA (HL)  85 -> C2 C", E().mem(0xC000, 0xC2).f(0x10).pc(0x0148).cycles(4)},
      {"LD B,81", E().b(0x81).pc(0x014A)},
      {"SRL B     81 -> 40 C (entra 0)", E().b(0x40).f(0x10).pc(0x014C).cycles(2)},
      {"LD C,01", E().c(0x01).pc(0x014E)},
      {"SRL C     01 -> 00 Z C", E().c(0x00).f(0x90).pc(0x0150).cycles(2)},
      {"LD D,FF", E().d(0xFF).pc(0x0152)},
      {"SRL D     FF -> 7F C", E().d(0x7F).f(0x10).pc(0x0154).cycles(2)},
      {"LD E,80", E().e(0x80).pc(0x0156)},
      {"SRL E     80 -> 40", E().e(0x40).f(0x00).pc(0x0158).cycles(2)},
      {"LD H,02", E().h(0x02).pc(0x015A)},
      {"SRL H     02 -> 01", E().h(0x01).f(0x00).pc(0x015C).cycles(2)},
      {"LD L,10", E().l(0x10).pc(0x015E)},
      {"SRL L     10 -> 08", E().l(0x08).f(0x00).pc(0x0160).cycles(2)},
      {"LD A,F0", E().a(0xF0).pc(0x0162)},
      {"SRL A     F0 -> 78", E().a(0x78).f(0x00).pc(0x0164).cycles(2)},
      {"LD HL,C000", E().hl(0xC000).pc(0x0167)},
      {"LD (HL),03", E().mem(0xC000, 0x03).pc(0x0169)},
      {"SRL (HL)  03 -> 01 C", E().mem(0xC000, 0x01).f(0x10).pc(0x016B).cycles(4)},
      {"SRL (HL)  01 -> 00 Z C", E().mem(0xC000, 0x00).f(0x90).pc(0x016D).cycles(4)},
  };
  return runSuiteMain(argc, argv, "cbshift", code, {}, steps);
}
