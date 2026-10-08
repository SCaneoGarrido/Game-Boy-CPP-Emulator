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

int CPU::op_ADD16bit_sp_n() {
  std::uint8_t raw_data = bus.read(PC);
  PC++;
  std::uint16_t extended_raw_data = static_cast<std::uint16_t>(raw_data);
  bool bit7 = (extended_raw_data & (1 << 7)) != 0;
  if (bit7) {
    extended_raw_data |= 0xFF00;
  }
  std::uint16_t result = static_cast<std::uint16_t>(SP + extended_raw_data);
  clearFlag(FLAG_Z);
  clearFlag(FLAG_N);

  if ((SP & 0x000F) + (raw_data & 0x0F) > 0x0F) {
    setFlag(FLAG_H);
  } else {
    clearFlag(FLAG_H);
  }

  if ((SP & 0x00FF) + (raw_data) > 0xFF) {
    setFlag(FLAG_C);
  } else {
    clearFlag(FLAG_C);
  }
  
  SP = result;

  return 4;
}
