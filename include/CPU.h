#ifndef CPU_H
#define CPU_H
#include "../include/MemoryBus.h"
#include "OpcodeLoaders.h"

#include <cstdint>
class CPU;

class CPU {
private: 
  BUS& bus;
  using Reg8Ptr = std::uint8_t CPU::*;
  // Special Registers
  std::uint16_t PC, SP;
  // ============================== Opcodes data ============================== 
  std::uint8_t current_opcode;
  using InstructionFunc = int (CPU::*)();
  InstructionFunc opcode_table[256];
  // ============================== Flags Mask========================================  
  const std::uint8_t FLAG_Z = 0x80;
  const std::uint8_t FLAG_N = 0x40;
  const std::uint8_t FLAG_H = 0x20;
  const std::uint8_t FLAG_C = 0x10;
  // ============================== registers & 16-bit methods ============================== 
  std::uint8_t A, B, C, D, E, H, L, F; 
  static const Reg8Ptr mapa_registros[8];
  std::uint16_t getPairedRegisters(std::uint8_t x_value, std::uint8_t y_value);
  void setPairedRegisters(std::uint8_t& x_reg, std::uint8_t& y_reg, std::uint16_t value);
  void initCpu();
  // ============================== CPU-INSTRUCTIONS =============================================
  template<std::uint8_t CPU::*registro_destino, bool hl_modified>
  int op_ld_n_imm8();
  template<std::uint8_t CPU::*registro_destino>
  int op_ld_n_hl();
  template<std::uint8_t CPU::*registro_origen>
  int op_ld_hl_n();
  template<bool its_writer ,std::uint8_t CPU::*xregistro, std::uint8_t CPU::*yregistro, int rr_operation>
  int op_ld_reg16_indirect();
  template<std::uint8_t CPU::*registro, bool hl_modified>
  int op_INC_n();
  template<std::uint8_t CPU::*registro, bool hl_modified>
  int op_DEC_n();
  template<std::uint8_t CPU::*xregistro, std::uint8_t CPU::*yregistro, bool sp_modified>
  int op_INC_nn();
  template<std::uint8_t CPU::*xregistro, std::uint8_t CPU::*yregistro, bool sp_modified>
  int op_DEC_nn();
  int op_ld_r1_r2();
  int b_illegal_opcode(); // EXCEPTION - PROTECCION DE opcode_table
  int b_nop();
  int b_stop();
  // =======================================================================================
  // ================================== FLAGS LOGICS =======================================
  void setFlag(std::uint8_t mask);
  void clearFlag(std::uint8_t mask);
  bool getFlag(std::uint8_t mask);
  bool checkHalfCarryAdd(std::uint8_t a, std::uint8_t b, std::uint8_t carry);
  bool checkCarryAdd(std::uint16_t a, std::uint16_t b, std::uint16_t carry);
  bool checkHalfCarrySub(std::uint8_t a, std::uint8_t b, std::uint8_t carry);
  bool checkCarrySub(std::uint16_t a, std::uint16_t b, std::uint16_t carry);
  // =================================== Friend functions =================================
  friend void OpcodeLoaders::load_ld_block(CPU& cpu);
  friend void OpcodeLoaders::load_INC_block(CPU& cpu);
  friend void OpcodeLoaders::load_DEC_block(CPU& cpu);
public:

  void loadOpcodes();
  CPU(BUS& _bus); // aqui necesitmaos pasar por referencia el BUS de memoria
  ~CPU();
  void cpuCycle();
  // LOGS CPU
  void showCPUINFO();
};
#endif // CPU_H
