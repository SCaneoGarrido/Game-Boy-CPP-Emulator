#include "../include/CPU.h"
#include <cstdint>

// ========================= Instruccion INC r8 ===============================
template<std::uint8_t CPU::*registro>
int CPU::op_INC_r8() {
  // Tomo el valor actual del registro 
  // al momento de la ejecucion de la Instruccion
  std::uint8_t original_value = this->*registro;
  // INC: Incrementa en 1 el registro
  std::uint8_t result = original_value + 1;
  
  // Evaluo FLAG Z 
  if (result == 0) setFlag(FLAG_Z); else clearFlag(FLAG_Z);
  clearFlag(FLAG_N);

  if ((original_value & 0x0F) == 0x0F) setFlag(FLAG_H); else clearFlag(FLAG_H);

  this->*registro = result;
  return 1;
}
template int CPU::op_INC_r8<&CPU::B>();
template int CPU::op_INC_r8<&CPU::C>();
template int CPU::op_INC_r8<&CPU::D>();
template int CPU::op_INC_r8<&CPU::E>();
template int CPU::op_INC_r8<&CPU::H>();
template int CPU::op_INC_r8<&CPU::L>();
template int CPU::op_INC_r8<&CPU::A>();
