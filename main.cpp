#include "./include/CPU.h"
#include "./include/MemoryBus.h"
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

// =============================================================================
// BANCO DE PRUEBAS (traza visual)
// -----------------------------------------------------------------------------
// Cada suite se ejecuta con un BUS y una CPU nuevos:
//   - Estado inicial: A..L = 0x00, F = 0x00, SP = 0x0000, PC = 0x0100.
//   - El codigo de la suite se copia en 0x0100.
//   - Los datos de la suite se copian en 0x0180 (DATA_ADDR). Los operandos
//     (HL)/(BC)/(DE) apuntan ahi porque la lectura de ROM es determinista y la
//     WRAM no (BUS::write es un stub y la WRAM no se inicializa).
//   - Se ejecutan EXACTAMENTE numInstr instrucciones.
// Los comentarios de cada instruccion indican el estado ESPERADO segun la
// especificacion del DMG (no segun la implementacion actual). Validar contra:
//   doc/opcodes_mapping/checklist_test_GB_emulator_26092026.txt
// Formato de flags: ZNHC  ->  F = Z(0x80) N(0x40) H(0x20) C(0x10)
//
// Uso:
//   ./DMGE           -> suites con opcodes registrados (ld, ld16, inc)
//   ./DMGE <suite>   -> ld | ld16 | inc | dec | add | sub | and | all
// Las suites dec/add/sub/and terminan hoy en "Opcode no implementado" porque
// sus loaders no se llaman en CPU::loadOpcodes() (ver checklist, B2).
// =============================================================================

static const std::uint16_t CODE_ADDR = 0x0100;
static const std::uint16_t DATA_ADDR = 0x0180;
static const std::size_t ROM_SIZE = 0x0200;

void runSuite(const std::string &nombre, const std::vector<std::uint8_t> &codigo,
              const std::vector<std::uint8_t> &datos, int numInstr) {
  std::cout << "\n##################################################\n"
            << "# SUITE: " << nombre << " (" << std::dec << numInstr
            << " instrucciones)\n"
            << "##################################################\n";

  std::vector<std::uint8_t> testRom(ROM_SIZE, 0x00);
  for (std::size_t i = 0; i < codigo.size(); i++)
    testRom[CODE_ADDR + i] = codigo[i];
  for (std::size_t i = 0; i < datos.size(); i++)
    testRom[DATA_ADDR + i] = datos[i];

  BUS bus;
  CPU cpu(bus);
  bus.load_rom(testRom);

  std::cout << "[estado inicial]\n";
  cpu.showCPUINFO();
  for (int i = 0; i < numInstr; i++) {
    std::cout << " ------------------------------------------------ \n"
              << "[" << std::dec << (i + 1) << "] ";
    cpu.cpuCycle();
    cpu.showCPUINFO();
  }
  std::cout << "[fin suite " << nombre << "] PC final esperado: 0x" << std::hex
            << std::uppercase << (CODE_ADDR + codigo.size()) << std::dec
            << "\n";
}

// -----------------------------------------------------------------------------
// SUITE ld: NOP, LD r8,imm8, LD r8,r8, LD r8,(HL), LD (HL),r8
// -----------------------------------------------------------------------------
void suiteLD() {
  std::vector<std::uint8_t> codigo = {
      0x00,       // 1.  NOP          -> sin cambios.                 PC=0101
      0x06, 0x11, // 2.  LD B,0x11    -> B=11                         PC=0103
      0x0E, 0x22, // 3.  LD C,0x22    -> C=22                         PC=0105
      0x16, 0x33, // 4.  LD D,0x33    -> D=33                         PC=0107
      0x1E, 0x44, // 5.  LD E,0x44    -> E=44                         PC=0109
      0x26, 0x01, // 6.  LD H,0x01    -> H=01                         PC=010B
      0x2E, 0x80, // 7.  LD L,0x80    -> L=80 (HL=0180)               PC=010D
      0x3E, 0x55, // 8.  LD A,0x55    -> A=55                         PC=010F
                  //     [FALLO CONOCIDO B1] hoy A queda en 00 y aparece
                  //     "Writing in: 180" (0x3E esta mapeado a LD (HL),n)
      0x78,       // 9.  LD A,B       -> A=11                         PC=0110
      0x41,       // 10. LD B,C       -> B=22                         PC=0111
      0x7F,       // 11. LD A,A       -> A=11 (sin cambios)           PC=0112
      0x4A,       // 12. LD C,D       -> C=33                         PC=0113
      0x53,       // 13. LD D,E       -> D=44                         PC=0114
      0x5F,       // 14. LD E,A       -> E=11                         PC=0115
      0x7E,       // 15. LD A,(HL)    -> A=A5 (ROM[0180])             PC=0116
      0x46,       // 16. LD B,(HL)    -> B=A5                         PC=0117
      0x70,       // 17. LD (HL),B    -> traza "Writing in: 180"      PC=0118
      0x77,       // 18. LD (HL),A    -> traza "Writing in: 180"      PC=0119
      0x6E,       // 19. LD L,(HL)    -> L=A5 (HL=01A5)               PC=011A
  };             // Flags: F=00 durante toda la suite (LD no modifica flags)
  std::vector<std::uint8_t> datos = {0xA5}; // 0x0180
  runSuite("ld", codigo, datos, 19);
}

// -----------------------------------------------------------------------------
// SUITE ld16: LD A,(rr) / LD (rr),A / (HL+) / (HL-)
// -----------------------------------------------------------------------------
void suiteLD16() {
  std::vector<std::uint8_t> codigo = {
      0x06, 0x01, // 1.  LD B,0x01    -> B=01                         PC=0102
      0x0E, 0x80, // 2.  LD C,0x80    -> C=80 (BC=0180)               PC=0104
      0x16, 0x01, // 3.  LD D,0x01    -> D=01                         PC=0106
      0x1E, 0x81, // 4.  LD E,0x81    -> E=81 (DE=0181)               PC=0108
      0x26, 0x01, // 5.  LD H,0x01    -> H=01                         PC=010A
      0x2E, 0x82, // 6.  LD L,0x82    -> L=82 (HL=0182)               PC=010C
      0x0A,       // 7.  LD A,(BC)    -> A=10                         PC=010D
      0x1A,       // 8.  LD A,(DE)    -> A=20                         PC=010E
      0x2A,       // 9.  LD A,(HL+)   -> A=30, HL=0183                PC=010F
      0x2A,       // 10. LD A,(HL+)   -> A=40, HL=0184                PC=0110
      0x3A,       // 11. LD A,(HL-)   -> A=50, HL=0183                PC=0111
      0x3A,       // 12. LD A,(HL-)   -> A=40, HL=0182                PC=0112
      0x02,       // 13. LD (BC),A    -> "Writing in: 180"            PC=0113
      0x12,       // 14. LD (DE),A    -> "Writing in: 181"            PC=0114
      0x22,       // 15. LD (HL+),A   -> "Writing in: 182", HL=0183   PC=0115
      0x32,       // 16. LD (HL-),A   -> "Writing in: 183", HL=0182   PC=0116
      0x2E, 0xFF, // 17. LD L,0xFF    -> HL=01FF                      PC=0118
      0x2A,       // 18. LD A,(HL+)   -> A=00, HL=0200 (acarreo L->H) PC=0119
  };             // Flags: F=00 durante toda la suite
  std::vector<std::uint8_t> datos = {0x10, 0x20, 0x30, 0x40, 0x50}; // 0x0180..
  runSuite("ld16", codigo, datos, 18);
}

// -----------------------------------------------------------------------------
// SUITE inc: INC r8 (Z0H, C intacto), INC rr / INC SP (sin flags), INC (HL)
// -----------------------------------------------------------------------------
void suiteINC() {
  std::vector<std::uint8_t> codigo = {
      0x06, 0x0F, // 1.  LD B,0x0F    -> B=0F                         PC=0102
      0x04,       // 2.  INC B        -> B=10  F=20 (--H-)            PC=0103
      0x04,       // 3.  INC B        -> B=11  F=00 (----)            PC=0104
      0x0E, 0xFF, // 4.  LD C,0xFF    -> C=FF                         PC=0106
      0x0C,       // 5.  INC C        -> C=00  F=A0 (Z-H-)            PC=0107
      0x16, 0x7F, // 6.  LD D,0x7F    -> D=7F                         PC=0109
      0x14,       // 7.  INC D        -> D=80  F=20 (--H-)            PC=010A
      0x1C,       // 8.  INC E        -> E=01  F=00                   PC=010B
      0x3C,       // 9.  INC A        -> A=01  F=00                   PC=010C
      0x06, 0x00, // 10. LD B,0x00    -> BC=0000                      PC=010E
      0x0E, 0xFF, // 11. LD C,0xFF    -> BC=00FF                      PC=0110
      0x2E, 0xFF, // 12. LD L,0xFF    -> L=FF                         PC=0112
      0x2C,       // 13. INC L        -> L=00  F=A0 (Z-H-)            PC=0113
      0x03,       // 14. INC BC       -> BC=0100 (B=01 C=00), F=A0 intacto
                  //                                                  PC=0114
      0x26, 0xFF, // 15. LD H,0xFF    -> H=FF                         PC=0116
      0x2E, 0xFF, // 16. LD L,0xFF    -> HL=FFFF                      PC=0118
      0x23,       // 17. INC HL       -> HL=0000 (desborde), F=A0     PC=0119
      0x16, 0x12, // 18. LD D,0x12    -> D=12                         PC=011B
      0x1E, 0x34, // 19. LD E,0x34    -> DE=1234                      PC=011D
      0x13,       // 20. INC DE       -> DE=1235, F=A0                PC=011E
      0x33,       // 21. INC SP       -> SP=0001, F=A0                PC=011F
      0x24,       // 22. INC H        -> H=01  F=00                   PC=0120
      0x2E, 0x80, // 23. LD L,0x80    -> HL=0180                      PC=0122
      0x34,       // 24. INC (HL)     -> lee 0F, escribe 10 en 0180
                  //                     "Writing in: 180"  F=20 (--H-) PC=0123
  };
  std::vector<std::uint8_t> datos = {0x0F}; // 0x0180
  runSuite("inc", codigo, datos, 24);
}

// -----------------------------------------------------------------------------
// SUITE dec: DEC r8 (Z1H, C intacto), DEC rr / DEC SP, DEC (HL)
// [B2] load_DEC_block no se llama -> hoy termina en la instruccion 2.
// [B5] flag H invertido en DEC r8.
// -----------------------------------------------------------------------------
void suiteDEC() {
  std::vector<std::uint8_t> codigo = {
      0x06, 0x10, // 1.  LD B,0x10    -> B=10                         PC=0102
      0x05,       // 2.  DEC B        -> B=0F  F=60 (-NH-)            PC=0103
      0x0E, 0x01, // 3.  LD C,0x01    -> C=01                         PC=0105
      0x0D,       // 4.  DEC C        -> C=00  F=C0 (ZN--)            PC=0106
      0x15,       // 5.  DEC D        -> D=FF  F=60 (-NH-)            PC=0107
      0x1E, 0x05, // 6.  LD E,0x05    -> E=05                         PC=0109
      0x1D,       // 7.  DEC E        -> E=04  F=40 (-N--)            PC=010A
      0x3D,       // 8.  DEC A        -> A=FF  F=60 (-NH-)            PC=010B
      0x25,       // 9.  DEC H        -> H=FF  F=60                   PC=010C
      0x2D,       // 10. DEC L        -> L=FF  F=60                   PC=010D
      0x0B,       // 11. DEC BC       -> BC 0F00->0EFF, F=60 intacto  PC=010E
      0x06, 0x00, // 12. LD B,0x00    -> BC=00FF                      PC=0110
      0x0E, 0x00, // 13. LD C,0x00    -> BC=0000                      PC=0112
      0x0B,       // 14. DEC BC       -> BC=FFFF (desborde)           PC=0113
      0x1B,       // 15. DEC DE       -> DE FF04->FF03                PC=0114
      0x2B,       // 16. DEC HL       -> HL FFFF->FFFE                PC=0115
      0x3B,       // 17. DEC SP       -> SP 0000->FFFF                PC=0116
      0x26, 0x01, // 18. LD H,0x01    -> H=01                         PC=0118
      0x2E, 0x80, // 19. LD L,0x80    -> HL=0180                      PC=011A
      0x35,       // 20. DEC (HL)     -> lee 01, escribe 00 en 0180
                  //                     "Writing in: 180"  F=C0 (ZN--) PC=011B
  };
  std::vector<std::uint8_t> datos = {0x01}; // 0x0180
  runSuite("dec", codigo, datos, 20);
}

// -----------------------------------------------------------------------------
// SUITE add: ADD A,n (0x80-0x87, 0xC6) y ADC A,n (0x88-0x8F, 0xCE)
// [B2] load_ADD_block no se llama -> hoy termina en la instruccion 4.
// -----------------------------------------------------------------------------
void suiteADD() {
  std::vector<std::uint8_t> codigo = {
      0x06, 0x0F, // 1.  LD B,0x0F    -> B=0F                         PC=0102
      0x78,       // 2.  LD A,B       -> A=0F                         PC=0103
      0x0E, 0x01, // 3.  LD C,0x01    -> C=01                         PC=0105
      0x81,       // 4.  ADD A,C      -> A=10  F=20 (--H-)            PC=0106
      0x16, 0x20, // 5.  LD D,0x20    -> D=20                         PC=0108
      0x82,       // 6.  ADD A,D      -> A=30  F=00                   PC=0109
      0x06, 0xF0, // 7.  LD B,0xF0    -> B=F0                         PC=010B
      0x80,       // 8.  ADD A,B      -> A=20  F=10 (---C)            PC=010C
      0x1E, 0xE0, // 9.  LD E,0xE0    -> E=E0                         PC=010E
      0x83,       // 10. ADD A,E      -> A=00  F=90 (Z--C)            PC=010F
      0x26, 0x01, // 11. LD H,0x01    -> H=01                         PC=0111
      0x2E, 0x80, // 12. LD L,0x80    -> HL=0180                      PC=0113
      0x86,       // 13. ADD A,(HL)   -> A=3A  F=00                   PC=0114
      0xC6, 0x08, // 14. ADD A,0x08   -> A=42  F=20 (--H-)            PC=0116
      0x87,       // 15. ADD A,A      -> A=84  F=00                   PC=0117
      0x84,       // 16. ADD A,H      -> A=85  F=00                   PC=0118
      0x85,       // 17. ADD A,L      -> A=05  F=10 (---C)            PC=0119
      // --- ADC (usa el carry previo) ---
      0x8F,       // 18. ADC A,A      -> 05+05+1: A=0B  F=00          PC=011A
      0x06, 0xF4, // 19. LD B,0xF4    -> B=F4                         PC=011C
      0x88,       // 20. ADC A,B      -> 0B+F4+0: A=FF  F=00          PC=011D
      0x89,       // 21. ADC A,C      -> FF+01+0: A=00  F=B0 (Z-HC)   PC=011E
      0xCE, 0x0F, // 22. ADC A,0x0F   -> 00+0F+1: A=10  F=20 (--H-)   PC=0120
      0x8E,       // 23. ADC A,(HL)   -> 10+3A+0: A=4A  F=00          PC=0121
  };
  std::vector<std::uint8_t> datos = {0x3A}; // 0x0180
  runSuite("add", codigo, datos, 23);
}

// -----------------------------------------------------------------------------
// SUITE sub: SUB n (0x90-0x97, 0xD6) y SBC A,n (0x98-0x9F, 0xDE)
// [B2] load_SUB_block no se llama. [B3] ademas registra SUB/SBC en los
// opcodes de ADD/AND -> hoy termina en la instruccion 4.
// -----------------------------------------------------------------------------
void suiteSUB() {
  std::vector<std::uint8_t> codigo = {
      0x06, 0x3E, // 1.  LD B,0x3E    -> B=3E                         PC=0102
      0x78,       // 2.  LD A,B       -> A=3E                         PC=0103
      0x0E, 0x3E, // 3.  LD C,0x3E    -> C=3E                         PC=0105
      0x91,       // 4.  SUB C        -> A=00  F=C0 (ZN--)            PC=0106
      0x06, 0x10, // 5.  LD B,0x10    -> B=10                         PC=0108
      0x78,       // 6.  LD A,B       -> A=10                         PC=0109
      0x16, 0x01, // 7.  LD D,0x01    -> D=01                         PC=010B
      0x92,       // 8.  SUB D        -> A=0F  F=60 (-NH-)            PC=010C
      0x1E, 0x20, // 9.  LD E,0x20    -> E=20                         PC=010E
      0x93,       // 10. SUB E        -> A=EF  F=50 (-N-C)            PC=010F
      0x97,       // 11. SUB A        -> A=00  F=C0 (ZN--)            PC=0110
      0x26, 0x01, // 12. LD H,0x01    -> H=01                         PC=0112
      0x2E, 0x80, // 13. LD L,0x80    -> HL=0180                      PC=0114
      0x96,       // 14. SUB (HL)     -> 00-01: A=FF  F=70 (-NHC)     PC=0115
      0xD6, 0x0F, // 15. SUB 0x0F     -> A=F0  F=40 (-N--)            PC=0117
      0xD6, 0xF1, // 16. SUB 0xF1     -> A=FF  F=70 (-NHC)            PC=0119
      // --- SBC (usa el carry previo) ---
      0x98,       // 17. SBC A,B      -> FF-10-1: A=EE  F=40 (-N--)   PC=011A
      0xD6, 0xF0, // 18. SUB 0xF0     -> A=FE  F=50 (-N-C)            PC=011C
      0x9F,       // 19. SBC A,A      -> FE-FE-1: A=FF  F=70 (-NHC)   PC=011D
      0xDE, 0x00, // 20. SBC A,0x00   -> FF-00-1: A=FE  F=40 (-N--)   PC=011F
      0x9E,       // 21. SBC A,(HL)   -> FE-01-0: A=FD  F=40 (-N--)   PC=0120
  };
  std::vector<std::uint8_t> datos = {0x01}; // 0x0180
  runSuite("sub", codigo, datos, 21);
}

// -----------------------------------------------------------------------------
// SUITE and: AND n (0xA0-0xA7, 0xE6)  ->  Z 0 1 0
// [B2] load_AND_block no se llama -> hoy termina en la instruccion 4.
// [B4] AND no limpia Z cuando el resultado es distinto de 0.
// -----------------------------------------------------------------------------
void suiteAND() {
  std::vector<std::uint8_t> codigo = {
      0x06, 0xF0, // 1.  LD B,0xF0    -> B=F0                         PC=0102
      0x78,       // 2.  LD A,B       -> A=F0                         PC=0103
      0x0E, 0x0F, // 3.  LD C,0x0F    -> C=0F                         PC=0105
      0xA1,       // 4.  AND C        -> A=00  F=A0 (Z-H-)            PC=0106
      0x06, 0xFF, // 5.  LD B,0xFF    -> B=FF                         PC=0108
      0x78,       // 6.  LD A,B       -> A=FF                         PC=0109
      0xA1,       // 7.  AND C        -> A=0F  F=20 (--H-)  [B4: hoy F=A0]
                  //                                                  PC=010A
      0x26, 0x01, // 8.  LD H,0x01    -> H=01                         PC=010C
      0x2E, 0x80, // 9.  LD L,0x80    -> HL=0180                      PC=010E
      0xA6,       // 10. AND (HL)     -> 0F&3C: A=0C  F=20            PC=010F
      0xE6, 0x04, // 11. AND 0x04     -> A=04  F=20                   PC=0111
      0xA7,       // 12. AND A        -> A=04  F=20                   PC=0112
      0xA0,       // 13. AND B        -> A=04  F=20                   PC=0113
      0xA2,       // 14. AND D        -> 04&00: A=00  F=A0 (Z-H-)     PC=0114
  };
  std::vector<std::uint8_t> datos = {0x3C}; // 0x0180
  runSuite("and", codigo, datos, 14);
}

int main(int argc, char *argv[]) {
  std::string suite = (argc > 1) ? argv[1] : "default";

  if (suite == "default") {
    // Solo familias con loader registrado en CPU::loadOpcodes()
    suiteLD();
    suiteLD16();
    suiteINC();
  } else if (suite == "ld") {
    suiteLD();
  } else if (suite == "ld16") {
    suiteLD16();
  } else if (suite == "inc") {
    suiteINC();
  } else if (suite == "dec") {
    suiteDEC();
  } else if (suite == "add" || suite == "adc") {
    suiteADD();
  } else if (suite == "sub" || suite == "sbc") {
    suiteSUB();
  } else if (suite == "and") {
    suiteAND();
  } else if (suite == "all") {
    // Se detiene en el primer opcode no registrado (b_illegal_opcode -> exit)
    suiteLD();
    suiteLD16();
    suiteINC();
    suiteDEC();
    suiteADD();
    suiteSUB();
    suiteAND();
  } else {
    std::cerr << "Suite desconocida: " << suite << "\n"
              << "Uso: DMGE [ld|ld16|inc|dec|add|sub|and|all]\n";
    return 1;
  }

  return 0;
}
