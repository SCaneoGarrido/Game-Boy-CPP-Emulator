#include "../../include/CPU.h"
#include <cstdint>

template <std::uint8_t CPU::*registro, bool hl_modified> 
int CPU::op_RLC_n() {
  std::uint8_t data;
  std::uint8_t bitout7;
  int Mcycles;

  if constexpr (hl_modified) {
    std::uint16_t HL_Direction = getPairedRegisters(H, L);
    data = bus.read(HL_Direction);
    bitout7 = (data >> 7) & 1;
    data = (data << 1) & 0xFF;
    data = data | bitout7;
    bus.write(HL_Direction, data);
    Mcycles = 4;
  } else {
    bitout7 = (this->*registro >> 7) & 1;
    data = (this->*registro << 1) & 0xFF;
    data = data | bitout7;
    this->*registro = data;
    Mcycles = 2;
  }

  clearFlag(FLAG_N);
  clearFlag(FLAG_H);

  if (data == 0)
    setFlag(FLAG_Z);
  else
    clearFlag(FLAG_Z);

  if (bitout7 == 1)
    setFlag(FLAG_C);
  if (bitout7 == 0)
    clearFlag(FLAG_C);

  return Mcycles;
}
template int CPU::op_RLC_n<&CPU::A, false>();
template int CPU::op_RLC_n<&CPU::B, false>();
template int CPU::op_RLC_n<&CPU::C, false>();
template int CPU::op_RLC_n<&CPU::D, false>();
template int CPU::op_RLC_n<&CPU::E, false>();
template int CPU::op_RLC_n<&CPU::H, false>();
template int CPU::op_RLC_n<&CPU::L, false>();
template int CPU::op_RLC_n<nullptr, true>();

template <std::uint8_t CPU::*registro, bool hl_modified> 
int CPU::op_RL_n() {
  std::uint8_t bitout7;
  std::uint8_t actual_CFlag_value = getFlag(FLAG_C) ? 1 : 0;
  std::uint8_t data;
  int Mcycles;
  if constexpr (hl_modified) {
    // Manejo del dato alojado en la direcion de memoria apuntada por HL
    std::uint16_t HL_Direction = getPairedRegisters(H, L);
    data = bus.read(HL_Direction);
    // Obtengo el bit 7 que se caera en la rotacion
    bitout7 = (data >> 7) & 1;
    data = (data << 1) & 0xFF;
    data = data | actual_CFlag_value;
    bus.write(HL_Direction, data);
    Mcycles = 4;
  } else {
    // Manejo de registro de 8 bits normal
    bitout7 = (this->*registro >> 7) & 1;
    data = (this->*registro << 1) & 0xFF;
    data = data | actual_CFlag_value;
    this->*registro = data;
    Mcycles = 2;
  }

  clearFlag(FLAG_H);
  clearFlag(FLAG_N);

  if (data == 0)
    setFlag(FLAG_Z);
  else
    clearFlag(FLAG_Z);
  if (bitout7 == 1)
    setFlag(FLAG_C);
  if (bitout7 == 0)
    clearFlag(FLAG_C);

  return Mcycles;
}
template int CPU::op_RL_n<&CPU::A, false>();
template int CPU::op_RL_n<&CPU::B, false>();
template int CPU::op_RL_n<&CPU::C, false>();
template int CPU::op_RL_n<&CPU::D, false>();
template int CPU::op_RL_n<&CPU::E, false>();
template int CPU::op_RL_n<&CPU::H, false>();
template int CPU::op_RL_n<&CPU::L, false>();
template int CPU::op_RL_n<nullptr, true>();
