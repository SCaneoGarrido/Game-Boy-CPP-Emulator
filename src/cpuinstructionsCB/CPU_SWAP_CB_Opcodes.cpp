#include "../../include/CPU.h"
#include <cstdint>

// ========================= Instruccion SWAP n ===============================
template <std::uint8_t CPU::*registro, bool hl_modified>
int CPU::op_cb_swap_n() { 
  std::uint8_t original_value;
  std::uint8_t result;
  int MCycles;

  if constexpr (hl_modified) {
    std::uint16_t HLMemory = getPairedRegisters(H, L);
    original_value = bus.read(HLMemory);
    result = static_cast<std::uint8_t>((original_value << 4) | (original_value >> 4));
    bus.write(HLMemory, result);
    MCycles = 4;
  } else {
    result = static_cast<std::uint8_t>((this->*registro << 4) | (this->*registro >> 4));
    this->*registro = result;
    MCycles = 2;
  }

  if (result == 0) setFlag(FLAG_Z);
  else clearFlag(FLAG_Z);

  clearFlag(FLAG_N);
  clearFlag(FLAG_H);
  clearFlag(FLAG_C);

  return MCycles;
}
template int CPU::op_cb_swap_n<&CPU::B, false>();
template int CPU::op_cb_swap_n<&CPU::C, false>();
template int CPU::op_cb_swap_n<&CPU::D, false>();
template int CPU::op_cb_swap_n<&CPU::E, false>();
template int CPU::op_cb_swap_n<&CPU::H, false>();
template int CPU::op_cb_swap_n<&CPU::L, false>();
template int CPU::op_cb_swap_n<&CPU::A, false>();
template int CPU::op_cb_swap_n<nullptr, true>();
