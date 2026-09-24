#include "../include/CPU.h"
#include <cstdint>

// ========================= Instruccion INC r8 ===============================
template<std::uint8_t CPU::*registro, bool hl_modified>
int CPU::op_INC_r8() {
  std::uint8_t original_value;
  std::uint8_t result;
  int MCycles;
  if constexpr (hl_modified) {
    std::uint16_t HLMemory = getPairedRegisters(H, L);
    original_value = bus.read(HLMemory);
    result = original_value + 1;
    bus.write(HLMemory, result);
    MCycles = 3; // 12 ciclos de reloj
  } else {
    original_value = this->*registro;
    result = original_value + 1;
    MCycles = 1; // 4 ciclos de reloj
  }
  // Evaluo FLAG Z 
  if (result == 0) setFlag(FLAG_Z); else clearFlag(FLAG_Z); clearFlag(FLAG_N);
  // Evaluo FLAG H
  if ((original_value & 0x0F) == 0x0F) setFlag(FLAG_H); else clearFlag(FLAG_H);
  return MCycles;
}
template int CPU::op_INC_r8<&CPU::B, false>();
template int CPU::op_INC_r8<&CPU::C, false>();
template int CPU::op_INC_r8<&CPU::D, false>();
template int CPU::op_INC_r8<&CPU::E, false>();
template int CPU::op_INC_r8<&CPU::H, false>();
template int CPU::op_INC_r8<&CPU::L, false>();
template int CPU::op_INC_r8<&CPU::A, false>();
template int CPU::op_INC_r8<nullptr, true>();
// ========================= Instruccion INC nn ===============================
template<std::uint8_t CPU::*xregistro, std::uint8_t CPU::*yregistro, bool sp_modified>
int CPU::op_INC_nn() {
  if (sp_modified) {
    SP++;
    return 2;
  }
  std::uint16_t paired16bitdata = getPairedRegisters(this->*xregistro, this->*yregistro);
  paired16bitdata += 1;
  setPairedRegisters(this->*xregistro, this->*yregistro, paired16bitdata);
  return 2;
}
template int CPU::op_INC_nn<&CPU::B, &CPU::C, false>();
template int CPU::op_INC_nn<&CPU::D, &CPU::E, false>();
template int CPU::op_INC_nn<&CPU::H, &CPU::L, false>();
template int CPU::op_INC_nn<nullptr, nullptr, true>();

