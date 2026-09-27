#include "../include/CPU.h"
#include "../include/OpcodeLoaders.h"
#include <cstdint>
#include <iostream>

void OpcodeLoaders::load_ld_block(CPU &cpu) {
  /*Funcion de carga masiva de instrucciones LD de 8 bits*/
  // ==========================================================
  // 1.- Carga de la matriz de 8 bits (0x40 a 0x7F)
  // ==========================================================
  for (int dest_row = 0; dest_row <= 7; dest_row++) {
    for (int src_col = 0; src_col <= 7; src_col++) {
      std::uint8_t opcode = 0x40 + (dest_row << 3) + src_col;
      // evaluo instruccion HALT
      if (dest_row == 6 && src_col == 6) {
        std::cout << "Instruccion HALT detectada, saltando al siguiente bloque"
                  << std::endl;
        continue;
      } else if (dest_row == 6) {
        switch (src_col) {
        // LD dest_row (HL), r8
        case 0: cpu.opcode_table[opcode] = &CPU::op_ld_hl_n<&CPU::B>;break;
        case 1: cpu.opcode_table[opcode] = &CPU::op_ld_hl_n<&CPU::C>;break;
        case 2: cpu.opcode_table[opcode] = &CPU::op_ld_hl_n<&CPU::D>;break;
        case 3: cpu.opcode_table[opcode] = &CPU::op_ld_hl_n<&CPU::E>;break;
        case 4: cpu.opcode_table[opcode] = &CPU::op_ld_hl_n<&CPU::H>;break;
        case 5: cpu.opcode_table[opcode] = &CPU::op_ld_hl_n<&CPU::L>;break;
        case 7: cpu.opcode_table[opcode] = &CPU::op_ld_hl_n<&CPU::A>;break;
        default:
          break;
        }
      } else if (src_col == 6) {
        switch (dest_row) {
        // LD r8, src_col (HL)
        case 0: cpu.opcode_table[opcode] = &CPU::op_ld_n_hl<&CPU::B>;break;
        case 1: cpu.opcode_table[opcode] = &CPU::op_ld_n_hl<&CPU::C>;break;
        case 2: cpu.opcode_table[opcode] = &CPU::op_ld_n_hl<&CPU::D>;break;
        case 3: cpu.opcode_table[opcode] = &CPU::op_ld_n_hl<&CPU::E>;break;
        case 4: cpu.opcode_table[opcode] = &CPU::op_ld_n_hl<&CPU::H>;break;
        case 5: cpu.opcode_table[opcode] = &CPU::op_ld_n_hl<&CPU::L>;break;
        case 7: cpu.opcode_table[opcode] = &CPU::op_ld_n_hl<&CPU::A>;break;
        default:
            break;
        }
      } else {
        cpu.opcode_table[opcode] = &CPU::op_ld_r1_r2;
      }
    }
  }

  // ==========================================================
  // 2.- Carga de instrucciones de 8 bits inmediatos (LD n, imm8)
  //==========================================================
  cpu.opcode_table[0x06] = &CPU::op_ld_n_imm8<&CPU::B, false>;  // LD B, d8
  cpu.opcode_table[0x0E] = &CPU::op_ld_n_imm8<&CPU::C, false>;  // LD C, d8
  cpu.opcode_table[0x16] = &CPU::op_ld_n_imm8<&CPU::D, false>;  // LD D, d8
  cpu.opcode_table[0x1E] = &CPU::op_ld_n_imm8<&CPU::E, false>;  // LD E, d8
  cpu.opcode_table[0x26] = &CPU::op_ld_n_imm8<&CPU::H, false>;  // LD H, d8
  cpu.opcode_table[0x2E] = &CPU::op_ld_n_imm8<&CPU::L, false>;  // LD L, d8
  cpu.opcode_table[0x3E] = &CPU::op_ld_n_imm8<&CPU::A, false>;  // LD A, d8
  cpu.opcode_table[0x36] = &CPU::op_ld_n_imm8<nullptr, true>;   // LD (HL), d8
  // ===========================================================================
  // 3. CARGA DE REGISTROS DE 16 BITS INDIRECTOS
  // ===========================================================================
  // Lecturas: LD A, (rr)
  cpu.opcode_table[0x0A] = &CPU::op_ld_reg16_indirect<false, &CPU::B, &CPU::C, 0>;
  cpu.opcode_table[0x1A] = &CPU::op_ld_reg16_indirect<false, &CPU::D, &CPU::E, 0>;
  cpu.opcode_table[0x2A] = &CPU::op_ld_reg16_indirect<false, &CPU::H, &CPU::L, 1>; // (HL+)
  cpu.opcode_table[0x3A] = &CPU::op_ld_reg16_indirect<false, &CPU::H, &CPU::L, 2>; // (HL-)

  // Escrituras: LD (rr), A
  cpu.opcode_table[0x02] = &CPU::op_ld_reg16_indirect<true, &CPU::B, &CPU::C, 0>;
  cpu.opcode_table[0x12] = &CPU::op_ld_reg16_indirect<true, &CPU::D, &CPU::E, 0>;
  cpu.opcode_table[0x22] = &CPU::op_ld_reg16_indirect<true, &CPU::H, &CPU::L, 1>;  // (HL+)
  cpu.opcode_table[0x32] = &CPU::op_ld_reg16_indirect<true, &CPU::H, &CPU::L, 2>;  // (HL-)
}

void OpcodeLoaders::load_ADD_block(CPU &cpu) {
  // ===========================================================================
  // 1. CARGA DE ADD A, n
  // ===========================================================================
  cpu.opcode_table[0x87] = &CPU::op_ADD_A_n<&CPU::A, false, false>;
  cpu.opcode_table[0x80] = &CPU::op_ADD_A_n<&CPU::B, false, false>;
  cpu.opcode_table[0x81] = &CPU::op_ADD_A_n<&CPU::C, false, false>;
  cpu.opcode_table[0x82] = &CPU::op_ADD_A_n<&CPU::D, false, false>;
  cpu.opcode_table[0x83] = &CPU::op_ADD_A_n<&CPU::E, false, false>;
  cpu.opcode_table[0x84] = &CPU::op_ADD_A_n<&CPU::H, false, false>;
  cpu.opcode_table[0x85] = &CPU::op_ADD_A_n<&CPU::L, false, false>;
  cpu.opcode_table[0x86] = &CPU::op_ADD_A_n<nullptr, true, false>;
  cpu.opcode_table[0xC6] = &CPU::op_ADD_A_n<nullptr, false, true>;
  // ===========================================================================
  // 2. CARGA DE ADC A, n
  // ===========================================================================
  cpu.opcode_table[0x8F] = &CPU::op_ADC_A_n<&CPU::A, false, false>;
  cpu.opcode_table[0x88] = &CPU::op_ADC_A_n<&CPU::B, false, false>;
  cpu.opcode_table[0x89] = &CPU::op_ADC_A_n<&CPU::C, false, false>;
  cpu.opcode_table[0x8A] = &CPU::op_ADC_A_n<&CPU::D, false, false>;
  cpu.opcode_table[0x8B] = &CPU::op_ADC_A_n<&CPU::E, false, false>;
  cpu.opcode_table[0x8C] = &CPU::op_ADC_A_n<&CPU::H, false, false>;
  cpu.opcode_table[0x8D] = &CPU::op_ADC_A_n<&CPU::L, false, false>;
  cpu.opcode_table[0x8E] = &CPU::op_ADC_A_n<nullptr, true, false>;
  cpu.opcode_table[0xCE] = &CPU::op_ADC_A_n<nullptr, false, true>;

}

void OpcodeLoaders::load_SUB_block(CPU &cpu) {
  // ===========================================================================
  // 1. CARGA DE SUB A, n
  // ===========================================================================
  cpu.opcode_table[0x97] = &CPU::op_SUB_A_n<&CPU::A, false, false>;
  cpu.opcode_table[0x90] = &CPU::op_SUB_A_n<&CPU::B, false, false>;
  cpu.opcode_table[0x91] = &CPU::op_SUB_A_n<&CPU::C, false, false>;
  cpu.opcode_table[0x92] = &CPU::op_SUB_A_n<&CPU::D, false, false>;
  cpu.opcode_table[0x93] = &CPU::op_SUB_A_n<&CPU::E, false, false>;
  cpu.opcode_table[0x94] = &CPU::op_SUB_A_n<&CPU::H, false, false>;
  cpu.opcode_table[0x95] = &CPU::op_SUB_A_n<&CPU::L, false, false>;
  cpu.opcode_table[0x96] = &CPU::op_SUB_A_n<nullptr, true, false>;
  cpu.opcode_table[0xD6] = &CPU::op_SUB_A_n<nullptr, false, true>;

  // ===========================================================================
  // 2. CARGA DE SBC A, n
  // ===========================================================================
  cpu.opcode_table[0x9f] = &CPU::op_SBC_A_n<&CPU::A, false, false>;
  cpu.opcode_table[0x98] = &CPU::op_SBC_A_n<&CPU::B, false, false>;
  cpu.opcode_table[0x99] = &CPU::op_SBC_A_n<&CPU::C, false, false>;
  cpu.opcode_table[0x9A] = &CPU::op_SBC_A_n<&CPU::D, false, false>;
  cpu.opcode_table[0x9B] = &CPU::op_SBC_A_n<&CPU::E, false, false>;
  cpu.opcode_table[0x9C] = &CPU::op_SBC_A_n<&CPU::H, false, false>;
  cpu.opcode_table[0x9D] = &CPU::op_SBC_A_n<&CPU::L, false, false>;
  cpu.opcode_table[0x9E] = &CPU::op_SBC_A_n<nullptr, true, false>;
  //cpu.opcode_table[0xE6] = &CPU::op_SBC_A_n<nullptr, false, true>; // existe ? 
}

void OpcodeLoaders::load_AND_block(CPU &cpu) {
  // ===========================================================================
  // 1. CARGA DE AND n
  // ===========================================================================
  cpu.opcode_table[0xA7] = &CPU::op_AND_n<&CPU::A, false, false>;
  cpu.opcode_table[0xA0] = &CPU::op_AND_n<&CPU::B, false, false>;
  cpu.opcode_table[0xA1] = &CPU::op_AND_n<&CPU::C, false, false>;
  cpu.opcode_table[0xA2] = &CPU::op_AND_n<&CPU::D, false, false>;
  cpu.opcode_table[0xA3] = &CPU::op_AND_n<&CPU::E, false, false>;
  cpu.opcode_table[0xA4] = &CPU::op_AND_n<&CPU::H, false, false>;
  cpu.opcode_table[0xA5] = &CPU::op_AND_n<&CPU::L, false, false>;
  cpu.opcode_table[0xA6] = &CPU::op_AND_n<nullptr, true, false>;
  cpu.opcode_table[0xE6] = &CPU::op_AND_n<nullptr, false, true>;
}

void OpcodeLoaders::load_INC_block(CPU &cpu) {
  // ===========================================================================
  // 1. CARGA DE INC n. operacion aritemtica de 8 BITS
  // ===========================================================================
  cpu.opcode_table[0x04] = &CPU::op_INC_n<&CPU::B, false>; // INC B
  cpu.opcode_table[0x0C] = &CPU::op_INC_n<&CPU::C, false>; // INC C
  cpu.opcode_table[0x14] = &CPU::op_INC_n<&CPU::D, false>; // INC D
  cpu.opcode_table[0x1C] = &CPU::op_INC_n<&CPU::E, false>; // INC E
  cpu.opcode_table[0x24] = &CPU::op_INC_n<&CPU::H, false>; // INC H
  cpu.opcode_table[0x2C] = &CPU::op_INC_n<&CPU::L, false>; // INC L
  cpu.opcode_table[0x3C] = &CPU::op_INC_n<&CPU::A, false>; // INC A
  cpu.opcode_table[0x34] = &CPU::op_INC_n<nullptr, true>;  // INC HL
  // ===========================================================================
  // 2. CARGA DE INC nn. operacion aritemtica de 16 BITS
  // ===========================================================================
  cpu.opcode_table[0x03] = &CPU::op_INC_nn<&CPU::B, &CPU::C, false>;
  cpu.opcode_table[0x13] = &CPU::op_INC_nn<&CPU::D, &CPU::E, false>;
  cpu.opcode_table[0x23] = &CPU::op_INC_nn<&CPU::H, &CPU::L, false>;
  cpu.opcode_table[0x33] = &CPU::op_INC_nn<nullptr, nullptr, true>;
}

void OpcodeLoaders::load_DEC_block(CPU &cpu) {
  // ===========================================================================
  // 1. DESCARGA DE DEC n. operacion aritemtica de 8 BITS
  // ===========================================================================
  cpu.opcode_table[0x05] = &CPU::op_DEC_n<&CPU::B, false>;
  cpu.opcode_table[0x0D] = &CPU::op_DEC_n<&CPU::C, false>;
  cpu.opcode_table[0x15] = &CPU::op_DEC_n<&CPU::D, false>;
  cpu.opcode_table[0x1D] = &CPU::op_DEC_n<&CPU::E, false>;
  cpu.opcode_table[0x25] = &CPU::op_DEC_n<&CPU::H, false>;
  cpu.opcode_table[0x2D] = &CPU::op_DEC_n<&CPU::L, false>;
  cpu.opcode_table[0x3D] = &CPU::op_DEC_n<&CPU::A, false>;
  cpu.opcode_table[0x35] = &CPU::op_DEC_n<nullptr, true>;
  // ===========================================================================
  // 2. DESCARGA DE DEC nn. operacion aritemtica de 16 BITS
  // ===========================================================================
  cpu.opcode_table[0x0B] = &CPU::op_DEC_nn<&CPU::B, &CPU::C, false>;
  cpu.opcode_table[0x1B] = &CPU::op_DEC_nn<&CPU::D, &CPU::E, false>;
  cpu.opcode_table[0x2B] = &CPU::op_DEC_nn<&CPU::H, &CPU::L, false>;
  cpu.opcode_table[0x3B] = &CPU::op_DEC_nn<nullptr, nullptr, true>;

}


