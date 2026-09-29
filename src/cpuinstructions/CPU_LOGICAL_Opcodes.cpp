#include "../../include/CPU.h"
#include <cstdint>
#include <execution>

// ========================= Instruccion AND n ===============================
template<std::uint8_t CPU::*registro, bool hl_modified, bool is_imm8>
int CPU::op_AND_n(){ 
  std::uint8_t n_value;
  int MCycle;

  if constexpr (hl_modified) {
    std::uint16_t HLMemory = getPairedRegisters(H, L);
    n_value = bus.read(HLMemory);
    A = A & n_value;
    MCycle = 2;
  }
  else if constexpr (is_imm8) {
    n_value = bus.read(PC);
    PC++;
    A = A & n_value;
    MCycle = 2;
  } else  {
    n_value = this->*registro;
    A = A & n_value;
    MCycle = 1;
  }

  clearFlag(FLAG_N);
  clearFlag(FLAG_C);
  if (A == 0)
    setFlag(FLAG_Z);
  else
   clearFlag(FLAG_Z);
  setFlag(FLAG_H);

  return MCycle;
}
template int CPU::op_AND_n<&CPU::A, false, false>();
template int CPU::op_AND_n<&CPU::B, false, false>();
template int CPU::op_AND_n<&CPU::C, false, false>();
template int CPU::op_AND_n<&CPU::D, false, false>();
template int CPU::op_AND_n<&CPU::E, false, false>();
template int CPU::op_AND_n<&CPU::H, false, false>();
template int CPU::op_AND_n<&CPU::L, false, false>();
template int CPU::op_AND_n<nullptr, true, false>();
template int CPU::op_AND_n<nullptr, false, true>();
// ========================= Instruccion OR n ===============================
template<std::uint8_t CPU::*registro, bool hl_modified, bool is_imm8>
int CPU::op_OR_n() {
  std::uint8_t n_value;
  int MCycle;

  if constexpr (hl_modified) {
    std::uint16_t HLMemory = getPairedRegisters(H, L);
    n_value = bus.read(HLMemory);
    A = A | n_value;
    MCycle = 2;
  } else if constexpr (is_imm8) {
    std::uint8_t n_value = bus.read(PC);
    PC++;
    A = A | n_value;
    MCycle = 2;
  } else {
    n_value = this->*registro;
    A = A | n_value;
    MCycle = 1;
  } 
  clearFlag(FLAG_N);
  clearFlag(FLAG_H);
  clearFlag(FLAG_C);
  if (A == 0) setFlag(FLAG_Z); 
  else clearFlag(FLAG_Z);
  return MCycle;
}
template int CPU::op_OR_n<&CPU::A, false, false>();
template int CPU::op_OR_n<&CPU::B, false, false>();
template int CPU::op_OR_n<&CPU::C, false, false>();
template int CPU::op_OR_n<&CPU::D, false, false>();
template int CPU::op_OR_n<&CPU::E, false, false>();
template int CPU::op_OR_n<&CPU::H, false, false>();
template int CPU::op_OR_n<&CPU::L, false, false>();
template int CPU::op_OR_n<nullptr, true, false>();
template int CPU::op_OR_n<nullptr, false, true>();
// ========================= Instruccion OR n ===============================
template<std::uint8_t CPU::*registro, bool hl_modified, bool is_imm8>
int CPU::op_XOR_n() {
  std::uint8_t n_value; 
  int MCycle;
  if constexpr (hl_modified) {
    std::uint16_t HLMemory = getPairedRegisters(H, L);
    n_value = bus.read(HLMemory);
    A = A ^ n_value;
    MCycle = 2;
  } else if constexpr (is_imm8) {
    n_value = bus.read(PC);
    PC++;
    A = A ^ n_value;
    MCycle = 2;
  } else {
    n_value = this->*registro;
    A = A ^ n_value;
    MCycle = 1;
  }
  clearFlag(FLAG_N);
  clearFlag(FLAG_H);
  clearFlag(FLAG_C);
  if (A == 0) setFlag(FLAG_Z);
  else clearFlag(FLAG_Z);
  return MCycle;
}
template int CPU::op_XOR_n<&CPU::A, false, false>();
template int CPU::op_XOR_n<&CPU::B, false, false>();
template int CPU::op_XOR_n<&CPU::C, false, false>();
template int CPU::op_XOR_n<&CPU::D, false, false>();
template int CPU::op_XOR_n<&CPU::E, false, false>();
template int CPU::op_XOR_n<&CPU::H, false, false>();
template int CPU::op_XOR_n<&CPU::L, false, false>();
template int CPU::op_XOR_n<nullptr, true, false>();
template int CPU::op_XOR_n<nullptr, false, true>();
// ========================= Instruccion CP n ===============================
template<std::uint8_t CPU::*registro, bool hl_modified, bool is_imm8>
int CPU::op_CP_n() {
  std::uint8_t n_value;
  int MCycle;
  if constexpr (hl_modified) {
    std::uint16_t HLMemory = getPairedRegisters(H, L);
    n_value = bus.read(HLMemory);
    MCycle = 2;
  } else if constexpr (is_imm8) {
    n_value = bus.read(PC);
    PC++;
    MCycle = 2; 
  } else {
    n_value = this->*registro; 
    MCycle = 1;
  }
  // valido flags aqui 
  if (A == n_value) setFlag(FLAG_Z);
  else clearFlag(FLAG_Z);

  setFlag(FLAG_N);

  if (checkHalfCarrySub(A, n_value, 0)) setFlag(FLAG_H);
  else clearFlag(FLAG_H);

  if (checkCarrySub(A, n_value, 0)) setFlag(FLAG_C);
  else clearFlag(FLAG_C);
  
  return MCycle;
}
template int CPU::op_CP_n<&CPU::A, false, false>();
template int CPU::op_CP_n<&CPU::B, false, false>();
template int CPU::op_CP_n<&CPU::C, false, false>();
template int CPU::op_CP_n<&CPU::D, false, false>();
template int CPU::op_CP_n<&CPU::E, false, false>();
template int CPU::op_CP_n<&CPU::H, false, false>();
template int CPU::op_CP_n<&CPU::L, false, false>();
template int CPU::op_CP_n<nullptr, true, false>();
template int CPU::op_CP_n<nullptr, false, true>();
