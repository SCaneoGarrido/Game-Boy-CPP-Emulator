#include "../../include/CPU.h"
#include <cstdint>
// ========================= Instruccion LD_n, nn
// ===============================
template <std::uint8_t CPU::*xregistro, std::uint8_t CPU::*yregistro,
          bool sp_modified>
int CPU::op_LD16b_n_imm16() {
  std::uint8_t low_byte = bus.read(PC);
  PC++;
  std::uint8_t high_byte = bus.read(PC);
  PC++;
  if constexpr (sp_modified) {
    // logica del load del Stack pointer
    SP = (static_cast<std::uint16_t>(high_byte) << 8) | low_byte;
    return 3;
  }
  setPairedRegisters(this->*xregistro, this->*yregistro,
                     (static_cast<std::uint16_t>(high_byte) << 8) | low_byte);
  return 3;
}
template int CPU::op_LD16b_n_imm16<&CPU::B, &CPU::C, false>();
template int CPU::op_LD16b_n_imm16<&CPU::D, &CPU::E, false>();
template int CPU::op_LD16b_n_imm16<&CPU::H, &CPU::L, false>();
template int CPU::op_LD16b_n_imm16<nullptr, nullptr, true>();
// ========================= Instruccion LD_n, nn
// ===============================
int CPU::op_ld_sp_hl() {
  SP = getPairedRegisters(H, L);
  return 2;
}
// ========================= Instruccion LDHL, SP+n
// ===============================
int CPU::op_ldhl_sp_n() {
  std::int8_t offset = static_cast<std::int8_t>(bus.read(PC));
  PC++;

  std::uint16_t final_result = SP + offset;
  setPairedRegisters(H, L, final_result);

  clearFlag(FLAG_Z);
  clearFlag(FLAG_N);

  std::uint8_t u_offset = static_cast<std::uint8_t>(offset);

  if (checkHalfCarryAdd(static_cast<std::uint8_t>(SP & 0xFF), u_offset, 0)) {
    setFlag(FLAG_H);
  } else {
    clearFlag(FLAG_H);
  }

  if (checkCarryAdd(static_cast<std::uint16_t>(SP & 0xFF),
                    static_cast<std::uint16_t>(u_offset), 0)) {
    setFlag(FLAG_C);
  } else {
    clearFlag(FLAG_C);
  }
  return 3;
}
// ========================= Instruccion LD (nn), SP ==============================
int CPU::op_ldnn_sp() {
  std::uint8_t low_byte = bus.read(PC);
  PC++;
  std::uint8_t high_byte = bus.read(PC);
  PC++;
  std::uint16_t target_address =
      (static_cast<std::uint16_t>(high_byte) << 8) | low_byte;
  std::uint8_t sp_low = SP & 0xFF;
  std::uint8_t sp_high = (SP >> 8) & 0xFF;
  // Escritra en el byte bajo
  bus.write(target_address, sp_low);
  // Escritra en el byte alto
  bus.write(target_address + 1, sp_high);
  return 5;
}
// ========================= Instruccion PUSH nn ===============================
template <std::uint8_t CPU::*hregistro, std::uint8_t CPU::*lregistro>
int CPU::op_PUSH_nn() {
  std::uint8_t high = this->*hregistro;
  std::uint8_t low  = this->*lregistro;
  std::uint16_t value = (high << 8) | low;
  push16(value);
  return 4;
}
template int CPU::op_PUSH_nn<&CPU::A, &CPU::F>();
template int CPU::op_PUSH_nn<&CPU::B, &CPU::C>();
template int CPU::op_PUSH_nn<&CPU::D, &CPU::E>();
template int CPU::op_PUSH_nn<&CPU::H, &CPU::L>();
// ========================= Instruccion POP nn ===============================
template <std::uint8_t CPU::*hregistro, std::uint8_t CPU::*lregistro>
int CPU::op_POP_nn() {
  std::uint16_t value = pop16();
  this->*lregistro = value & 0xFF;
  this->*hregistro = (value >> 8) & 0xFF;
  if constexpr (lregistro == &CPU::F) {
    F = F & 0xF0;
  }
  return 3;
}
template int CPU::op_POP_nn<&CPU::A, &CPU::F>();
template int CPU::op_POP_nn<&CPU::B, &CPU::C>();
template int CPU::op_POP_nn<&CPU::D, &CPU::E>();
template int CPU::op_POP_nn<&CPU::H, &CPU::L>();
