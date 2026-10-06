#include "../../include/CPU.h"
#include <cstdint>
template<std::uint8_t CPU::*hregistro, std::uint8_t CPU::*lregistro, bool  sp_modified>
int CPU::op_ADD16BIT_HL_n() { 
  std::uint16_t dest_data = getPairedRegisters(H, L);
  std::uint16_t source_data; 
  if constexpr (sp_modified) {
    source_data = SP;
    setPairedRegisters(H, L, static_cast<std::uint16_t>(dest_data + source_data));
  } else {
    source_data = getPairedRegisters(this->*hregistro, this->*lregistro);
    setPairedRegisters(H, L, static_cast<std::uint16_t>(dest_data + source_data));
  }

  clearFlag(FLAG_N);
  if (check16bitCarryAdd(dest_data, source_data))
    setFlag(FLAG_C);
  else
   clearFlag(FLAG_C);

  if (check16bitHalfCarryAdd(dest_data, source_data))
    setFlag(FLAG_H);
  else
    clearFlag(FLAG_H);

  return 2;
}
template int CPU::op_ADD16BIT_HL_n<&CPU::B, &CPU::C, false>(); // Opcode 0x09 ADD HL, BC
template int CPU::op_ADD16BIT_HL_n<&CPU::D, &CPU::E, false>(); // Opcode 0x19 ADD HL, DE
template int CPU::op_ADD16BIT_HL_n<&CPU::H, &CPU::L, false>(); // Opcode 0x29 ADD HL, HL
template int CPU::op_ADD16BIT_HL_n<nullptr, nullptr, true>();  // Opcode 0x39 ADD HL, SP
