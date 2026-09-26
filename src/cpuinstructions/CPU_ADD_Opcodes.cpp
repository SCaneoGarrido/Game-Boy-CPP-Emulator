#include "../../include/CPU.h"
#include <cstdint>
// ========================= Instruccion ADD A, n ===============================
template <std::uint8_t CPU::*registro, bool hl_modified, bool is_imm8>
int CPU::op_ADD_A_n() {
  std::uint8_t original_value = A;
  std::uint8_t n_value;
  std::uint16_t result; // sumaremos A + n
  int MCycles;
  if constexpr (hl_modified) {
    std::uint16_t HLMemory = getPairedRegisters(H, L);
    n_value = bus.read(HLMemory);
    result = original_value + n_value;
    A = static_cast<std::uint8_t>(result);
    MCycles = 2;
  } else if constexpr (is_imm8) {
    n_value = bus.read(PC);
    PC++;
    result = original_value + n_value;
    A = static_cast<std::uint8_t>(result);
    MCycles = 2;
  } else {
    n_value = this->*registro;
    result = original_value + n_value;
    A = static_cast<std::uint8_t>(result);
    MCycles = 1;
  }
  clearFlag(FLAG_N);
  if (A == 0)
    setFlag(FLAG_Z);
  else
    clearFlag(FLAG_Z);
  if (checkHalfCarryAdd(original_value, n_value, 0)) {
    setFlag(FLAG_H);
  } else {
    clearFlag(FLAG_H);
  }
  if (checkCarryAdd(original_value, n_value, 0)) {
    setFlag(FLAG_C);
  } else {
    clearFlag(FLAG_C);
  }
  return MCycles;
}
template int CPU::op_ADD_A_n<&CPU::A, false, false>();
template int CPU::op_ADD_A_n<&CPU::B, false, false>();
template int CPU::op_ADD_A_n<&CPU::C, false, false>();
template int CPU::op_ADD_A_n<&CPU::D, false, false>();
template int CPU::op_ADD_A_n<&CPU::E, false, false>();
template int CPU::op_ADD_A_n<&CPU::H, false, false>();
template int CPU::op_ADD_A_n<&CPU::L, false, false>();
template int CPU::op_ADD_A_n<nullptr, true, false>();
template int CPU::op_ADD_A_n<nullptr, false, true>();
// ========================= Instruccion ADC A, n ===============================
template <std::uint8_t CPU::*registro, bool hl_modified, bool is_imm8>
int CPU::op_ADC_A_n() {
  std::uint8_t original_value = A;
  std::uint8_t previous_carry = getFlag(FLAG_C) ? 1 : 0;
  std::uint8_t n_value;
  std::uint16_t result;
  int MCycles;
  if constexpr (hl_modified) {
    std::uint16_t HLMemory = getPairedRegisters(H, L);
    n_value = bus.read(HLMemory);
    result = original_value + n_value + previous_carry;
    A = static_cast<std::uint8_t>(result);
    MCycles = 2;
  } else if constexpr (is_imm8) {
    n_value = bus.read(PC);
    PC++;
    result = original_value + n_value + previous_carry;
    A = static_cast<std::uint8_t>(result);
    MCycles = 2;
  } else {
    n_value = this->*registro;
    result = original_value + n_value + previous_carry;
    A = static_cast<std::uint8_t>(result);
    MCycles = 1;
  }
  clearFlag(FLAG_N);
  if (A == 0)
    setFlag(FLAG_Z);
  else
    clearFlag(FLAG_Z);
  if (checkHalfCarryAdd(original_value, n_value, previous_carry)) {
    setFlag(FLAG_H);
  } else {
    clearFlag(FLAG_H);
  }
  if (checkCarryAdd(original_value, n_value, previous_carry)) {
    setFlag(FLAG_C);
  } else {
    clearFlag(FLAG_C);
  }
  return MCycles;
}
template int CPU::op_ADC_A_n<&CPU::A, false, false>();
template int CPU::op_ADC_A_n<&CPU::B, false, false>();
template int CPU::op_ADC_A_n<&CPU::C, false, false>();
template int CPU::op_ADC_A_n<&CPU::D, false, false>();
template int CPU::op_ADC_A_n<&CPU::E, false, false>();
template int CPU::op_ADC_A_n<&CPU::H, false, false>();
template int CPU::op_ADC_A_n<&CPU::L, false, false>();
template int CPU::op_ADC_A_n<nullptr, true, false>();
template int CPU::op_ADC_A_n<nullptr, false, true>();

