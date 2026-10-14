#include "../../include/CPU.h"
#include <cstdint>

template<std::uint8_t CPU::*registro, bool hl_modified>
int CPU::op_SLA_n() {
  std::uint8_t data;
  std::uint8_t bit7;
  int Mcycles;

  if constexpr (hl_modified) { 
    std::uint16_t HL_Direction = getPairedRegisters(H, L);
    data = bus.read(HL_Direction);
    bit7 = (data >> 7) & 1;
    data = (data << 1) & 0xFF;
    bus.write(HL_Direction, data);
    Mcycles = 4;
  } else {
    bit7 =(this->*registro >> 7) & 1;
    data = (this->*registro << 1) & 0xFF;
    this->*registro = data;
    Mcycles = 2;
  }

  clearFlag(FLAG_H);
  clearFlag(FLAG_N);

  if (data == 0) setFlag(FLAG_Z); else clearFlag(FLAG_Z);
  if (bit7 == 1) setFlag(FLAG_C);
  if (bit7 == 0) clearFlag(FLAG_C);

  return Mcycles;
}
template int CPU::op_SLA_n<&CPU::A, false>();
template int CPU::op_SLA_n<&CPU::B, false>();
template int CPU::op_SLA_n<&CPU::C, false>();
template int CPU::op_SLA_n<&CPU::D, false>();
template int CPU::op_SLA_n<&CPU::E, false>();
template int CPU::op_SLA_n<&CPU::H, false>();
template int CPU::op_SLA_n<&CPU::L, false>();
template int CPU::op_SLA_n<nullptr, true>();

template<std::uint8_t CPU::*registro, bool hl_modified>
int CPU::op_SRA_n() {
  std::uint8_t data;
  std::uint8_t bit0;
  std::uint8_t bit7;
  int Mcycles;

  if constexpr (hl_modified) {
    std::uint16_t HL_Direction = getPairedRegisters(H, L);
    data = bus.read(HL_Direction);
    bit0 = data & 1;
    bit7 = (data >> 7) & 1;
    data = (data >> 1) & 0xFF;
    data = data | (bit7 << 7);
    bus.write(HL_Direction, data);
    Mcycles = 4;
  } else {
    bit0 = this->*registro & 1;
    bit7 = (this->*registro >> 7) & 1;
    data = (this->*registro >> 1) & 0xFF;
    data = data | (bit7 << 7);
    this->*registro = data;
    Mcycles = 2;
  }

  clearFlag(FLAG_H);
  clearFlag(FLAG_N);
  
  if (data == 0) setFlag(FLAG_Z); else clearFlag(FLAG_Z);
  if (bit0 == 1) setFlag(FLAG_C);
  if (bit0 == 0) clearFlag(FLAG_C);

  return Mcycles;
}
template int CPU::op_SRA_n<&CPU::A, false>();
template int CPU::op_SRA_n<&CPU::B, false>();
template int CPU::op_SRA_n<&CPU::C, false>();
template int CPU::op_SRA_n<&CPU::D, false>();
template int CPU::op_SRA_n<&CPU::E, false>();
template int CPU::op_SRA_n<&CPU::H, false>();
template int CPU::op_SRA_n<&CPU::L, false>();
template int CPU::op_SRA_n<nullptr, true>();

template<std::uint8_t CPU::*registro, bool hl_modified>
int CPU::op_SRL_n() {
  std::uint8_t data;
  std::uint8_t bit0;
  int Mcycles;

  if constexpr (hl_modified) {
    std::uint16_t HL_Direction = getPairedRegisters(H, L);
    data = bus.read(HL_Direction);
    bit0 = data & 1;
    data = (data >> 1) & 0xFF;
    bus.write(HL_Direction, data);
    Mcycles = 4;
  } else {
    bit0 = this->*registro & 1;
    data = (this->*registro >> 1) & 0xFF;
    this->*registro = data;
    Mcycles = 2;
  }
  clearFlag(FLAG_H);
  clearFlag(FLAG_N);
  
  if (data == 0) setFlag(FLAG_Z); else clearFlag(FLAG_Z);
  if (bit0 == 1) setFlag(FLAG_C);
  if (bit0 == 0) clearFlag(FLAG_C);

  return Mcycles;
}

template int CPU::op_SRL_n<&CPU::A, false>();
template int CPU::op_SRL_n<&CPU::B, false>();
template int CPU::op_SRL_n<&CPU::C, false>();
template int CPU::op_SRL_n<&CPU::D, false>();
template int CPU::op_SRL_n<&CPU::E, false>();
template int CPU::op_SRL_n<&CPU::H, false>();
template int CPU::op_SRL_n<&CPU::L, false>();
template int CPU::op_SRL_n<nullptr, true>();

