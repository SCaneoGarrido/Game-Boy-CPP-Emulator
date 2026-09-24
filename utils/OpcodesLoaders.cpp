#include "../include/CPU.h"
#include "../include/OpcodeLoaders.h"
#include <cstdint>
#include <iostream>

void OpcodeLoaders::load_ld_8bits_block(CPU &cpu) {
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
        switch (dest_row) {
        // LD dest_row (HL), r8
        case 0: cpu.opcode_table[opcode] = &CPU::op_ld_hl_r8<&CPU::B>;break;
        case 1: cpu.opcode_table[opcode] = &CPU::op_ld_hl_r8<&CPU::C>;break;
        case 2: cpu.opcode_table[opcode] = &CPU::op_ld_hl_r8<&CPU::D>;break;
        case 3: cpu.opcode_table[opcode] = &CPU::op_ld_hl_r8<&CPU::E>;break;
        case 4: cpu.opcode_table[opcode] = &CPU::op_ld_hl_r8<&CPU::H>;break;
        case 5: cpu.opcode_table[opcode] = &CPU::op_ld_hl_r8<&CPU::L>;break;
        case 7: cpu.opcode_table[opcode] = &CPU::op_ld_hl_r8<&CPU::A>;break;
        default:
          break;
        }
      } else if (src_col == 6) {
        switch (dest_row) {
        // LD r8, src_col (HL)
        case 0: cpu.opcode_table[opcode] = &CPU::op_ld_r8_hl<&CPU::B>;break;
        case 1: cpu.opcode_table[opcode] = &CPU::op_ld_r8_hl<&CPU::C>;break;
        case 2: cpu.opcode_table[opcode] = &CPU::op_ld_r8_hl<&CPU::D>;break;
        case 3: cpu.opcode_table[opcode] = &CPU::op_ld_r8_hl<&CPU::E>;break;
        case 4: cpu.opcode_table[opcode] = &CPU::op_ld_r8_hl<&CPU::H>;break;
        case 5: cpu.opcode_table[opcode] = &CPU::op_ld_r8_hl<&CPU::L>;break;
        case 7: cpu.opcode_table[opcode] = &CPU::op_ld_r8_hl<&CPU::A>;break;
        default:
            break;
        }
      } else {
        cpu.opcode_table[opcode] = &CPU::op_ld_r8_r8;
      }
    }
  }

  // ==========================================================
  // 2.- Carga de instrucciones de 8 bits inmediatos (LD r8, imm8)
  //==========================================================
  cpu.opcode_table[0x06] = &CPU::op_ld_r8_imm8<&CPU::B>;
  cpu.opcode_table[0x0E] = &CPU::op_ld_r8_imm8<&CPU::C>;
  cpu.opcode_table[0x16] = &CPU::op_ld_r8_imm8<&CPU::D>;
  cpu.opcode_table[0x1E] = &CPU::op_ld_r8_imm8<&CPU::E>;
  cpu.opcode_table[0x26] = &CPU::op_ld_r8_imm8<&CPU::H>;
  cpu.opcode_table[0x2E] = &CPU::op_ld_r8_imm8<&CPU::L>;
  cpu.opcode_table[0x3E] = &CPU::op_ld_r8_imm8<&CPU::A>;

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


void OpcodeLoaders::load_ld_INC_block(CPU &cpu) {
  // ===========================================================================
  // 1. CARGA DE INC r8. operacion aritemtica de 8 BITS
  // ===========================================================================
  cpu.opcode_table[0x04] = &CPU::op_INC_r8<&CPU::B, false>; // INC B
  cpu.opcode_table[0x0C] = &CPU::op_INC_r8<&CPU::C, false>; // INC C
  cpu.opcode_table[0x14] = &CPU::op_INC_r8<&CPU::D, false>; // INC D
  cpu.opcode_table[0x1C] = &CPU::op_INC_r8<&CPU::E, false>; // INC E
  cpu.opcode_table[0x24] = &CPU::op_INC_r8<&CPU::H, false>; // INC H
  cpu.opcode_table[0x2C] = &CPU::op_INC_r8<&CPU::L, false>; // INC L
  cpu.opcode_table[0x3C] = &CPU::op_INC_r8<&CPU::A, false>; // INC A
  cpu.opcode_table[0x34] = &CPU::op_INC_r8<nullptr, true>;  // INC HL
   // ===========================================================================
  // 2. CARGA DE INC nn. operacion aritemtica de 16 BITS
  // ===========================================================================
  cpu.opcode_table[0x03] = &CPU::op_INC_nn<&CPU::B, &CPU::C, false>;
  cpu.opcode_table[0x13] = &CPU::op_INC_nn<&CPU::D, &CPU::E, false>;
  cpu.opcode_table[0x23] = &CPU::op_INC_nn<&CPU::H, &CPU::L, false>;
  cpu.opcode_table[0x33] = &CPU::op_INC_nn<nullptr, nullptr, true>;
}
