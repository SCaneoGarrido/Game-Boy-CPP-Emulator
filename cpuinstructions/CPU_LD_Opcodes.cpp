#include "../include/CPU.h"
#include <cstdint>

// ========================= Instruccion LD_r8_imm8 ===============================
template <std::uint8_t CPU::*registro_destino, bool hl_modified>
int CPU::op_ld_n_imm8() {
  std::uint8_t dato = bus.read(PC);
  PC++;
  if constexpr (hl_modified) {
    std::uint16_t HLMemory = getPairedRegisters(H, L);
    bus.write(HLMemory, dato);
    return 3;
  }
  this->*registro_destino = dato;
  return 2;
}
template int CPU::op_ld_n_imm8<&CPU::A, false>();
template int CPU::op_ld_n_imm8<&CPU::B, false>();
template int CPU::op_ld_n_imm8<&CPU::C, false>();
template int CPU::op_ld_n_imm8<&CPU::D, false>();
template int CPU::op_ld_n_imm8<&CPU::E, false>();
template int CPU::op_ld_n_imm8<&CPU::H, false>();
template int CPU::op_ld_n_imm8<&CPU::L, false>();
template int CPU::op_ld_n_imm8<nullptr, true>();
// ========================= Instruccion LD_r1_r2 ===============================
int CPU::op_ld_r1_r2() {
  std::uint8_t dest_idx = (current_opcode >> 3) & 0x07;
  std::uint8_t src_idx = current_opcode & 0x07;
  this->*mapa_registros[dest_idx] = this->*mapa_registros[src_idx];
  return 1;
}
// ========================= Instruccion LD_n_hl ===============================
template <std::uint8_t CPU::*registro_destino>
int CPU::op_ld_n_hl() {
  // Obtengo la direccion de memmoria
  std::uint16_t memory_direction = getPairedRegisters(H, L);
  // Leo la direccion de memmoria
  std::uint8_t data = bus.read(memory_direction);
  // Cargo en el destrino el dato leido de la memoria.
  this->*registro_destino = data;
  return 2; // 1 M-Cycle = 4 T-Cycle
}
template int CPU::op_ld_n_hl<&CPU::A>();
template int CPU::op_ld_n_hl<&CPU::B>();
template int CPU::op_ld_n_hl<&CPU::C>();
template int CPU::op_ld_n_hl<&CPU::D>();
template int CPU::op_ld_n_hl<&CPU::E>();
template int CPU::op_ld_n_hl<&CPU::H>();
template int CPU::op_ld_n_hl<&CPU::L>();
// ========================= Instruccion LD_hl_n ===============================
template <std::uint8_t CPU::*registro_origen>
int CPU::op_ld_hl_n() {
  std::uint16_t direction = getPairedRegisters(H, L); 
  std::uint8_t data = this->*registro_origen;
  bus.write(direction, data);
  return 2;
}
template int CPU::op_ld_hl_n<&CPU::A>();
template int CPU::op_ld_hl_n<&CPU::B>();
template int CPU::op_ld_hl_n<&CPU::C>();
template int CPU::op_ld_hl_n<&CPU::D>();
template int CPU::op_ld_hl_n<&CPU::E>();
template int CPU::op_ld_hl_n<&CPU::H>();
template int CPU::op_ld_hl_n<&CPU::L>();
// ========================= Instruccion LD_reg16_indirect ===============================
template <bool its_writer, std::uint8_t CPU::*xregistro, std::uint8_t CPU::*yregistro, int rr_operation>
int CPU::op_ld_reg16_indirect() {
  std::uint16_t direction = getPairedRegisters(this->*xregistro, this->*yregistro);
  if (its_writer) {
    bus.write(direction, this->A);
  } else {
    this->A = bus.read(direction);
  }

  if (rr_operation == 1)
    direction += 1;
  if (rr_operation == 2)
      direction -= 1;
  if (rr_operation != 0)
    setPairedRegisters(this->*xregistro, this->*yregistro, direction);
  
  return 2;
}

// ==================== LECTURAS: LD A, (rr) ====================
// Parámetros: <es_escritura=false, registro_alto, registro_bajo, operacion>
template int CPU::op_ld_reg16_indirect<false, &CPU::B, &CPU::C, 0>(); // Opcode 0x0A  LD A, (BC)
template int CPU::op_ld_reg16_indirect<false, &CPU::D, &CPU::E, 0>(); // Opcode 0x1A  LD A, (DE)
template int CPU::op_ld_reg16_indirect<false, &CPU::H, &CPU::L, 1>(); // Opcode 0x2A  LD A, (HL+)
template int CPU::op_ld_reg16_indirect<false, &CPU::H, &CPU::L, 2>(); // Opcode 0x3A  LD A, (HL-)

// ==================== ESCRITURAS: LD (rr), A ====================
// Parámetros: <es_escritura=true, registro_alto, registro_bajo, operacion>
template int CPU::op_ld_reg16_indirect<true, &CPU::B, &CPU::C, 0>(); // Opcode 0x02       LD (BC), A
template int CPU::op_ld_reg16_indirect<true, &CPU::D, &CPU::E, 0>(); // Opcode 0x12 (DE)  LD (DE), A
template int CPU::op_ld_reg16_indirect<true, &CPU::H, &CPU::L, 1>(); // Opcode 0x22 (HL+) LD (HL+), A
template int CPU::op_ld_reg16_indirect<true, &CPU::H, &CPU::L, 2>(); // Opcode 0x32 (HL-) LD (HL-), A




