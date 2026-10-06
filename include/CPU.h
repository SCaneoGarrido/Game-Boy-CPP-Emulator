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
  int MCycles;
  void initCpu();
  // ============================== Opcodes data ============================================= 
  std::uint8_t current_opcode;
  using InstructionFunc = int (CPU::*)();
  InstructionFunc opcode_table[256];
  // ============================== Flags Mask ==============================  
  const std::uint8_t FLAG_Z = 0x80;
  const std::uint8_t FLAG_N = 0x40;
  const std::uint8_t FLAG_H = 0x20;
  const std::uint8_t FLAG_C = 0x10;
  // ============================== registers & 16-bit methods ============================== 
  std::uint8_t A, B, C, D, E, H, L, F; 
  static const Reg8Ptr mapa_registros[8];
  std::uint16_t getPairedRegisters(std::uint8_t x_value, std::uint8_t y_value);
  void setPairedRegisters(std::uint8_t& x_reg, std::uint8_t& y_reg, std::uint16_t value);
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
  template<std::uint8_t CPU::*registro, bool hl_modified, bool is_imm8>
  int op_ADD_A_n();
  template<std::uint8_t CPU::*registro, bool hl_modified, bool is_imm8>
  int op_ADC_A_n();
  template<std::uint8_t CPU::*registro, bool hl_modified, bool is_imm8>
  int op_SUB_A_n();
  template<std::uint8_t CPU::*registro, bool hl_modified, bool is_imm8>
  int op_SBC_A_n();
  template<std::uint8_t CPU::*registro, bool hl_modified, bool is_imm8>
  int op_AND_n();
  template<std::uint8_t CPU::*registro, bool hl_modified, bool is_imm8>
  int op_OR_n();
  template<std::uint8_t CPU::*registro, bool hl_modified, bool is_imm8>
  int op_XOR_n();
  template<std::uint8_t CPU::*registro, bool hl_modified, bool is_imm8>
  int op_CP_n();
  template<std::uint8_t CPU::*xregistro, std::uint8_t CPU::*yregistro, bool sp_modified>
  int op_LD16b_n_imm16();
  template<std::uint8_t CPU::*hregistro, std::uint8_t CPU::*lregistro>
  int op_PUSH_nn();
  template<std::uint8_t CPU::*hregistro, std::uint8_t CPU::*lregistro>
  int op_POP_nn();
  template<std::uint8_t CPU::*hregistro, std::uint8_t CPU::*lregistro, bool sp_modified>
  int op_ADD16BIT_HL_n();
  // ============================== NO TEMPLATE CPU-INSTRUCTIONS =============================================
  int op_ld_r1_r2();
  int op_ld_a_C(); // direccionamiento por registro C
  int op_ld_c_a();
  int op_ldh_n_a();
  int op_ldh_a_n();
  int op_ld_a16_a();
  int op_ld_a_a16();
  int b_illegal_opcode(); // EXCEPTION - PROTECCION DE opcode_table
  int b_nop();
  int b_stop();
  int op_ld_sp_hl();
  int op_ldhl_sp_n();
  int op_ldnn_sp(); // LD (nn) SP
  // ================================== FLAGS LOGICS =======================================
  void setFlag(std::uint8_t mask);
  void clearFlag(std::uint8_t mask);
  bool getFlag(std::uint8_t mask);
  bool checkHalfCarryAdd(std::uint8_t a, std::uint8_t b, std::uint8_t carry);
  bool checkCarryAdd(std::uint16_t a, std::uint16_t b, std::uint16_t carry);
  bool checkHalfCarrySub(std::uint8_t a, std::uint8_t b, std::uint8_t carry);
  bool checkCarrySub(std::uint16_t a, std::uint16_t b, std::uint16_t carry);
  bool check16bitHalfCarryAdd(std::uint16_t a, std::uint16_t b);
  bool check16bitCarryAdd(std::uint16_t a, std::uint16_t b);
  // =================================== Friend functions =================================
  friend void OpcodeLoaders::load_ld_block(CPU& cpu);
  friend void OpcodeLoaders::load_INC_block(CPU& cpu);
  friend void OpcodeLoaders::load_DEC_block(CPU& cpu);
  friend void OpcodeLoaders::load_ADD_block(CPU &cpu);
  friend void OpcodeLoaders::load_SUB_block(CPU &cpu);
  friend void OpcodeLoaders::load_LOGICAL_block(CPU &cpu);
  friend void OpcodeLoaders::load_LD16BITS_block(CPU &cpu);
  friend void OpcodeLoaders::load_ADD16BITS_block(CPU &cpu);
public:
  void loadOpcodes();
  CPU(BUS& _bus); // aqui necesitmaos pasar por referencia el BUS de memoria
  ~CPU();
  void cpuCycle();
  // LOGS CPU
  void showCPUINFO();
};
#endif // CPU_H
