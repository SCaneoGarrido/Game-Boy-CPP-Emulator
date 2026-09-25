#include "../include/CPU.h"
#include <cstdint>


// ========================= Instruccion DEC n ===============================
template <std::uint8_t CPU::*registro, bool hl_modified>
int CPU::op_DEC_n() {
  std::uint8_t original_value;
  std::uint8_t result;
  int MCycles;

  if constexpr (hl_modified) {
    std::uint16_t HLMemory = getPairedRegisters(H, L);
    original_value = bus.read(HLMemory);
    result = original_value - 1;
    bus.write(HLMemory, result);
    MCycles = 3;
  } else {
    original_value = this->*registro;
    result = original_value - 1;
    this->*registro = result;
    MCycles = 1;
  }
  
  setFlag(FLAG_N); // setting first, bc its a decrement operation
  if (result == 0) setFlag(FLAG_Z); else clearFlag(FLAG_Z);
  if ((original_value & 0x0F) == 0) clearFlag(FLAG_H); else setFlag(FLAG_H);

  return MCycles;
}
template int CPU::op_DEC_n<&CPU::B, false>();
template int CPU::op_DEC_n<&CPU::C, false>();
template int CPU::op_DEC_n<&CPU::D, false>();
template int CPU::op_DEC_n<&CPU::E, false>();
template int CPU::op_DEC_n<&CPU::H, false>();
template int CPU::op_DEC_n<&CPU::L, false>();
template int CPU::op_DEC_n<&CPU::A, false>();
template int CPU::op_DEC_n<nullptr, true>();
// ======================== INSTREUCCION DEC nn ==================================
template <std::uint8_t CPU::*xregistro, std::uint8_t CPU::*yregistro, bool sp_modified>
int CPU::op_DEC_nn() {
  if constexpr (sp_modified) {
    SP--;
    return 2;
  }

  std::uint16_t paired16bitdata = getPairedRegisters(this->*xregistro, this->*yregistro);
  paired16bitdata -= 1;
  setPairedRegisters(this->*xregistro, this->*yregistro, paired16bitdata);
  return 2;
}
template int CPU::op_DEC_nn<&CPU::B, &CPU::C, false>();
template int CPU::op_DEC_nn<&CPU::D, &CPU::E, false>();
template int CPU::op_DEC_nn<&CPU::H, &CPU::L, false>();
template int CPU::op_DEC_nn<nullptr, nullptr, true>();

