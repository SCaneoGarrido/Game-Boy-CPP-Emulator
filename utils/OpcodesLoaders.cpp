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
  cpu.opcode_table[0x0A] = &CPU::op_ld_reg16_indirect<false, &CPU::B, &CPU::C, 0>; // LD A, (BC)
  cpu.opcode_table[0x1A] = &CPU::op_ld_reg16_indirect<false, &CPU::D, &CPU::E, 0>; // LD A, (DE)
  cpu.opcode_table[0x2A] = &CPU::op_ld_reg16_indirect<false, &CPU::H, &CPU::L, 1>; // LD A, (HL+)
  cpu.opcode_table[0x3A] = &CPU::op_ld_reg16_indirect<false, &CPU::H, &CPU::L, 2>; // LD A, (HL-)

  // Escrituras: LD (rr), A
  cpu.opcode_table[0x02] = &CPU::op_ld_reg16_indirect<true, &CPU::B, &CPU::C, 0>;  // LD  (BC),  A
  cpu.opcode_table[0x12] = &CPU::op_ld_reg16_indirect<true, &CPU::D, &CPU::E, 0>;  // LD  (DE),  A
  cpu.opcode_table[0x22] = &CPU::op_ld_reg16_indirect<true, &CPU::H, &CPU::L, 1>;  // LD  (HL+), A
  cpu.opcode_table[0x32] = &CPU::op_ld_reg16_indirect<true, &CPU::H, &CPU::L, 2>;  // LD  (HL-), A
  // ===========================================================================
  // 4. CARGA DE INSTRUCCIONES ESPECIALES DE I/O Y DIRECCIONAMIENTO ABSOLUTO (8-bit)
  // ===========================================================================
  cpu.opcode_table[0xE2] = &CPU::op_ld_c_a;   // LD (C), A   - Escribe A en $FF00 + C
  cpu.opcode_table[0xF2] = &CPU::op_ld_a_C;   // LD A, (C)   - Lee desde $FF00 + C hacia A
  cpu.opcode_table[0xE0] = &CPU::op_ldh_n_a;  // LDH (a8), A - Escribe A en $FF00 + inmediato d8
  cpu.opcode_table[0xF0] = &CPU::op_ldh_a_n;  // LDH A, (a8) - Lee desde $FF00 + inmediato d8 hacia A
  cpu.opcode_table[0xEA] = &CPU::op_ld_a16_a; // LD (a16), A - Escribe A en dirección absoluta de 16 bits
  cpu.opcode_table[0xFA] = &CPU::op_ld_a_a16; // LD A, (a16) - Lee desde dirección absoluta de 16 bits hacia A
}

void OpcodeLoaders::load_LD16BITS_block(CPU &cpu) {
  // ===========================================================================
  // 1. CARGA DE LD n, nn
  // ===========================================================================
  cpu.opcode_table[0x01] = &CPU::op_LD16b_n_imm16<&CPU::B, &CPU::C, false>;  // LD   BC, d16
  cpu.opcode_table[0x11] = &CPU::op_LD16b_n_imm16<&CPU::D, &CPU::E, false>;  // LD   DE, d16
  cpu.opcode_table[0x21] = &CPU::op_LD16b_n_imm16<&CPU::H, &CPU::L, false>;  // LD   HL, d16
  cpu.opcode_table[0x31] = &CPU::op_LD16b_n_imm16<nullptr, nullptr, true>;   // LD   SP, d16
  // ================================== Operacion LD SP, HL ==================================
  cpu.opcode_table[0xF9] = &CPU::op_ld_sp_hl;
  // ================================== Operacion LD HL, Sp+n ==================================
  cpu.opcode_table[0xF8] = &CPU::op_ldhl_sp_n;
  // ========================= Instruccion LD (nn), SP ==============================
  cpu.opcode_table[0x08] = &CPU::op_ldnn_sp;
  // ===========================================================================
  // 2. CARGA DE PUSH nn
  // ===========================================================================
  cpu.opcode_table[0xF5] = &CPU::op_PUSH_nn<&CPU::A, &CPU::F>;  // PUSH AF, SP
  cpu.opcode_table[0xC5] = &CPU::op_PUSH_nn<&CPU::B, &CPU::C>;  // PUSH BC, SP
  cpu.opcode_table[0xD5] = &CPU::op_PUSH_nn<&CPU::D, &CPU::E>;  // PUSH DE, SP
  cpu.opcode_table[0xE5] = &CPU::op_PUSH_nn<&CPU::H, &CPU::L>;  // PUSH HL, SP
  // ===========================================================================
  // 3. CARGA DE POP nn
  // ===========================================================================
  cpu.opcode_table[0xF1] = &CPU::op_POP_nn<&CPU::A, &CPU::F>;  // POP AF, SP
  cpu.opcode_table[0xC1] = &CPU::op_POP_nn<&CPU::B, &CPU::C>;  // POP BC, SP
  cpu.opcode_table[0xD1] = &CPU::op_POP_nn<&CPU::D, &CPU::E>;  // POP DE, SP
  cpu.opcode_table[0xE1] = &CPU::op_POP_nn<&CPU::H, &CPU::L>;  // POP HL, SP
}

void OpcodeLoaders::load_ADD_block(CPU &cpu) {
  // ===========================================================================
  // 1. CARGA DE ADD A, n
  // ===========================================================================
  cpu.opcode_table[0x87] = &CPU::op_ADD_A_n<&CPU::A, false, false>; // ADD A, A
  cpu.opcode_table[0x80] = &CPU::op_ADD_A_n<&CPU::B, false, false>; // ADD A, B
  cpu.opcode_table[0x81] = &CPU::op_ADD_A_n<&CPU::C, false, false>; // ADD A, C
  cpu.opcode_table[0x82] = &CPU::op_ADD_A_n<&CPU::D, false, false>; // ADD A, D
  cpu.opcode_table[0x83] = &CPU::op_ADD_A_n<&CPU::E, false, false>; // ADD A, E
  cpu.opcode_table[0x84] = &CPU::op_ADD_A_n<&CPU::H, false, false>; // ADD A, H
  cpu.opcode_table[0x85] = &CPU::op_ADD_A_n<&CPU::L, false, false>; // ADD A, L
  cpu.opcode_table[0x86] = &CPU::op_ADD_A_n<nullptr, true, false>;  // ADD A, HL
  cpu.opcode_table[0xC6] = &CPU::op_ADD_A_n<nullptr, false, true>;  // ADD A, d8
  // ===========================================================================
  // 2. CARGA DE ADC A, n
  // ===========================================================================
  cpu.opcode_table[0x8F] = &CPU::op_ADC_A_n<&CPU::A, false, false>; // ADC A, A 
  cpu.opcode_table[0x88] = &CPU::op_ADC_A_n<&CPU::B, false, false>; // ADC A, B 
  cpu.opcode_table[0x89] = &CPU::op_ADC_A_n<&CPU::C, false, false>; // ADC A, C 
  cpu.opcode_table[0x8A] = &CPU::op_ADC_A_n<&CPU::D, false, false>; // ADC A, D 
  cpu.opcode_table[0x8B] = &CPU::op_ADC_A_n<&CPU::E, false, false>; // ADC A, E 
  cpu.opcode_table[0x8C] = &CPU::op_ADC_A_n<&CPU::H, false, false>; // ADC A, H 
  cpu.opcode_table[0x8D] = &CPU::op_ADC_A_n<&CPU::L, false, false>; // ADC A, L 
  cpu.opcode_table[0x8E] = &CPU::op_ADC_A_n<nullptr, true, false>;  // ADC A, HL 
  cpu.opcode_table[0xCE] = &CPU::op_ADC_A_n<nullptr, false, true>;  // ADC A, d8

}

void OpcodeLoaders::load_SUB_block(CPU &cpu) {
  // ===========================================================================
  // 1. CARGA DE SUB A, n
  // ===========================================================================
  cpu.opcode_table[0x97] = &CPU::op_SUB_A_n<&CPU::A, false, false>; // SUB A
  cpu.opcode_table[0x90] = &CPU::op_SUB_A_n<&CPU::B, false, false>; // SUB B
  cpu.opcode_table[0x91] = &CPU::op_SUB_A_n<&CPU::C, false, false>; // SUB C
  cpu.opcode_table[0x92] = &CPU::op_SUB_A_n<&CPU::D, false, false>; // SUB D
  cpu.opcode_table[0x93] = &CPU::op_SUB_A_n<&CPU::E, false, false>; // SUB E
  cpu.opcode_table[0x94] = &CPU::op_SUB_A_n<&CPU::H, false, false>; // SUB H
  cpu.opcode_table[0x95] = &CPU::op_SUB_A_n<&CPU::L, false, false>; // SUB L
  cpu.opcode_table[0x96] = &CPU::op_SUB_A_n<nullptr, true, false>;  // SUB (HL)
  cpu.opcode_table[0xD6] = &CPU::op_SUB_A_n<nullptr, false, true>;  // SUB d8

  // ===========================================================================
  // 2. CARGA DE SBC A, n
  // ===========================================================================
  cpu.opcode_table[0x9f] = &CPU::op_SBC_A_n<&CPU::A, false, false>; // SBC A, A 
  cpu.opcode_table[0x98] = &CPU::op_SBC_A_n<&CPU::B, false, false>; // SBC A, B 
  cpu.opcode_table[0x99] = &CPU::op_SBC_A_n<&CPU::C, false, false>; // SBC A, C 
  cpu.opcode_table[0x9A] = &CPU::op_SBC_A_n<&CPU::D, false, false>; // SBC A, D 
  cpu.opcode_table[0x9B] = &CPU::op_SBC_A_n<&CPU::E, false, false>; // SBC A, E 
  cpu.opcode_table[0x9C] = &CPU::op_SBC_A_n<&CPU::H, false, false>; // SBC A, H 
  cpu.opcode_table[0x9D] = &CPU::op_SBC_A_n<&CPU::L, false, false>; // SBC A, L 
  cpu.opcode_table[0x9E] = &CPU::op_SBC_A_n<nullptr, true, false>;  // SBC A, HL
  cpu.opcode_table[0xDE] = &CPU::op_SBC_A_n<nullptr, false, true>;  // SBC A, d8 
}

void OpcodeLoaders::load_LOGICAL_block(CPU &cpu) {
  // ===========================================================================
  // 1. CARGA DE AND n
  // ===========================================================================
  cpu.opcode_table[0xA7] = &CPU::op_AND_n<&CPU::A, false, false>; // AND A, A 
  cpu.opcode_table[0xA0] = &CPU::op_AND_n<&CPU::B, false, false>; // AND A, B 
  cpu.opcode_table[0xA1] = &CPU::op_AND_n<&CPU::C, false, false>; // AND A, C 
  cpu.opcode_table[0xA2] = &CPU::op_AND_n<&CPU::D, false, false>; // AND A, D 
  cpu.opcode_table[0xA3] = &CPU::op_AND_n<&CPU::E, false, false>; // AND A, E 
  cpu.opcode_table[0xA4] = &CPU::op_AND_n<&CPU::H, false, false>; // AND A, H 
  cpu.opcode_table[0xA5] = &CPU::op_AND_n<&CPU::L, false, false>; // AND A, L 
  cpu.opcode_table[0xA6] = &CPU::op_AND_n<nullptr, true, false>;  // AND A, HL
  cpu.opcode_table[0xE6] = &CPU::op_AND_n<nullptr, false, true>;  // AND A, d8
  // ===========================================================================
  // 2. CARGA DE OR n
  // ===========================================================================
  cpu.opcode_table[0xB7] = &CPU::op_OR_n<&CPU::A, false, false>; // OR A, A 
  cpu.opcode_table[0xB0] = &CPU::op_OR_n<&CPU::B, false, false>; // OR A, B 
  cpu.opcode_table[0xB1] = &CPU::op_OR_n<&CPU::C, false, false>; // OR A, C 
  cpu.opcode_table[0xB2] = &CPU::op_OR_n<&CPU::D, false, false>; // OR A, D 
  cpu.opcode_table[0xB3] = &CPU::op_OR_n<&CPU::E, false, false>; // OR A, E 
  cpu.opcode_table[0xB4] = &CPU::op_OR_n<&CPU::H, false, false>; // OR A, H 
  cpu.opcode_table[0xB5] = &CPU::op_OR_n<&CPU::L, false, false>; // OR A, L 
  cpu.opcode_table[0xB6] = &CPU::op_OR_n<nullptr, true, false>;  // OR A, HL
  cpu.opcode_table[0xF6] = &CPU::op_OR_n<nullptr, false, true>;  // OR A, d8
  // ===========================================================================
  // 3. CARGA DE XOR n
  // ===========================================================================
  cpu.opcode_table[0xAF] = &CPU::op_XOR_n<&CPU::A, false, false>; // XOR A, A 
  cpu.opcode_table[0xA8] = &CPU::op_XOR_n<&CPU::B, false, false>; // XOR A, B 
  cpu.opcode_table[0xA9] = &CPU::op_XOR_n<&CPU::C, false, false>; // XOR A, C 
  cpu.opcode_table[0xAA] = &CPU::op_XOR_n<&CPU::D, false, false>; // XOR A, D 
  cpu.opcode_table[0xAB] = &CPU::op_XOR_n<&CPU::E, false, false>; // XOR A, E 
  cpu.opcode_table[0xAC] = &CPU::op_XOR_n<&CPU::H, false, false>; // XOR A, H 
  cpu.opcode_table[0xAD] = &CPU::op_XOR_n<&CPU::L, false, false>; // XOR A, L 
  cpu.opcode_table[0xAE] = &CPU::op_XOR_n<nullptr, true, false>;  // XOR A, HL
  cpu.opcode_table[0xEE] = &CPU::op_XOR_n<nullptr, false, true>;  // XOR A, d8
  // ===========================================================================
  // 4. CARGA DE CP n
  // ===========================================================================
  cpu.opcode_table[0xBF] = &CPU::op_CP_n<&CPU::A, false, false>; // CP A, A 
  cpu.opcode_table[0xB8] = &CPU::op_CP_n<&CPU::B, false, false>; // CP A, B 
  cpu.opcode_table[0xB9] = &CPU::op_CP_n<&CPU::C, false, false>; // CP A, C 
  cpu.opcode_table[0xBA] = &CPU::op_CP_n<&CPU::D, false, false>; // CP A, D 
  cpu.opcode_table[0xBB] = &CPU::op_CP_n<&CPU::E, false, false>; // CP A, E 
  cpu.opcode_table[0xBC] = &CPU::op_CP_n<&CPU::H, false, false>; // CP A, H 
  cpu.opcode_table[0xBD] = &CPU::op_CP_n<&CPU::L, false, false>; // CP A, L 
  cpu.opcode_table[0xBE] = &CPU::op_CP_n<nullptr, true, false>;  // CP A, HL
  cpu.opcode_table[0xFE] = &CPU::op_CP_n<nullptr, false, true>;  // CP A, d8
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
  cpu.opcode_table[0x03] = &CPU::op_INC_nn<&CPU::B, &CPU::C, false>; // INC BC
  cpu.opcode_table[0x13] = &CPU::op_INC_nn<&CPU::D, &CPU::E, false>; // INC DE
  cpu.opcode_table[0x23] = &CPU::op_INC_nn<&CPU::H, &CPU::L, false>; // INC HL
  cpu.opcode_table[0x33] = &CPU::op_INC_nn<nullptr, nullptr, true>;  // INC SP
}

void OpcodeLoaders::load_DEC_block(CPU &cpu) {
  // ===========================================================================
  // 1. DESCARGA DE DEC n. operacion aritemtica de 8 BITS
  // ===========================================================================
  cpu.opcode_table[0x05] = &CPU::op_DEC_n<&CPU::B, false>; // DEC B 
  cpu.opcode_table[0x0D] = &CPU::op_DEC_n<&CPU::C, false>; // DEC C 
  cpu.opcode_table[0x15] = &CPU::op_DEC_n<&CPU::D, false>; // DEC D 
  cpu.opcode_table[0x1D] = &CPU::op_DEC_n<&CPU::E, false>; // DEC E 
  cpu.opcode_table[0x25] = &CPU::op_DEC_n<&CPU::H, false>; // DEC H 
  cpu.opcode_table[0x2D] = &CPU::op_DEC_n<&CPU::L, false>; // DEC L 
  cpu.opcode_table[0x3D] = &CPU::op_DEC_n<&CPU::A, false>; // DEC A 
  cpu.opcode_table[0x35] = &CPU::op_DEC_n<nullptr, true>;  // DEC HL
  // ===========================================================================
  // 2. DESCARGA DE DEC nn. operacion aritemtica de 16 BITS
  // ===========================================================================
  cpu.opcode_table[0x0B] = &CPU::op_DEC_nn<&CPU::B, &CPU::C, false>;  // DEC BC
  cpu.opcode_table[0x1B] = &CPU::op_DEC_nn<&CPU::D, &CPU::E, false>;  // DEC DE
  cpu.opcode_table[0x2B] = &CPU::op_DEC_nn<&CPU::H, &CPU::L, false>;  // DEC HL
  cpu.opcode_table[0x3B] = &CPU::op_DEC_nn<nullptr, nullptr, true>;   // DEC SP

}

void OpcodeLoaders::load_ADD16BITS_block(CPU &cpu) {
  // ===========================================================================
  // 1. CARGA DE ADD HL, n
  // ===========================================================================
  cpu.opcode_table[0x09] = &CPU::op_ADD16BIT_HL_n<&CPU::B, &CPU::C, false>;  // ADD HL, BC 
  cpu.opcode_table[0x19] = &CPU::op_ADD16BIT_HL_n<&CPU::D, &CPU::E, false>;  // ADD HL, DE 
  cpu.opcode_table[0x29] = &CPU::op_ADD16BIT_HL_n<&CPU::H, &CPU::L, false>;  // ADD HL, HL 
  cpu.opcode_table[0x39] = &CPU::op_ADD16BIT_HL_n<nullptr, nullptr, true>;   // ADD HL, SP 

  cpu.opcode_table[0xE8] = &CPU::op_ADD16bit_sp_n;
}


// CB PREFIX
void OpcodeLoaders::load_SWAP_n_cb_block(CPU &cpu) {
  // ===========================================================================
  // 1. CARGA DE SWAP n
  // ===========================================================================
  cpu.opcode_table_cb[0x30] = &CPU::op_cb_swap_n<&CPU::B, false>; // INC B
  cpu.opcode_table_cb[0x31] = &CPU::op_cb_swap_n<&CPU::C, false>; // INC C
  cpu.opcode_table_cb[0x32] = &CPU::op_cb_swap_n<&CPU::D, false>; // INC D
  cpu.opcode_table_cb[0x33] = &CPU::op_cb_swap_n<&CPU::E, false>; // INC E
  cpu.opcode_table_cb[0x34] = &CPU::op_cb_swap_n<&CPU::H, false>; // INC H
  cpu.opcode_table_cb[0x35] = &CPU::op_cb_swap_n<&CPU::L, false>; // INC L
  cpu.opcode_table_cb[0x37] = &CPU::op_cb_swap_n<&CPU::A, false>; // INC A
  cpu.opcode_table_cb[0x36] = &CPU::op_cb_swap_n<nullptr, true>;  // INC HL
  
}
void OpcodeLoaders::load_MISSCELLANEOUS_block(CPU &cpu) {
  cpu.opcode_table[0x27] = &CPU::op_DAA;
  cpu.opcode_table[0x2F] = &CPU::op_CPL;
  cpu.opcode_table[0x3F] = &CPU::op_CCF;
  cpu.opcode_table[0x37] = &CPU::op_SCF;
  cpu.opcode_table[0x00] = &CPU::op_nop;
  cpu.opcode_table[0x76] = &CPU::op_halt;
  cpu.opcode_table[0x10] = &CPU::op_stop;
  cpu.opcode_table[0xF3] = &CPU::op_di;
  cpu.opcode_table[0xFB] = &CPU::op_ei;
}


