#ifndef CPU_H
#define CPU_H
#include "../include/MemoryBus.h"
#include "OpcodeLoaders.h"

#include <cstdint>
class CPU;
namespace OpcodeLoaders {
  // Definicion de la Definicion
  void load_ld_block(CPU& cpu);

}
class CPU {
private:
  // Flags
  // Special Registers
  std::uint16_t PC, SP;
  // opcode
  std::uint8_t current_opcode;
  // Flags masks
  const std::uint8_t FLAG_Z = 0x80;
  const std::uint8_t FLAG_N = 0x40;
  const std::uint8_t FLAG_H = 0x20;
  const std::uint8_t FLAG_C = 0x10;
  // MemoryBus
  BUS& bus;
  void initCpu(); 
  //void clearResources();
  // Opcode methods
  void fetchOpcode(); // Busca la instruccion almadenada en la direccion de PC
  void decodeExecute();
  using InstructionFunc = int (CPU::*)();
  InstructionFunc opcode_table[256];

  //std::uint16_t getHlDirection();
  std::uint16_t getPairedRegisters(std::uint8_t x_value, std::uint8_t y_value);
  void setPairedRegisters(std::uint8_t& x_reg, std::uint8_t& y_reg, std::uint16_t value);

  // ============================== CPU-INSTRUCTIONS =============================================
  template<std::uint8_t CPU::*registro_destino>
  int op_ld_r8_imm8();
  template<std::uint8_t CPU::*registro_destino>
  int op_ld_r8_hl();
  template<std::uint8_t CPU::*registro_origen>
  int op_ld_hl_r8();
  template<bool its_writer ,std::uint8_t CPU::*xregistro, std::uint8_t CPU::*yregistro, int rr_operation>
  int op_ld_reg16_indirect();
  int op_ld_r8_r8();
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
  friend void OpcodeLoaders::load_ld_8bits_block(CPU& cpu); 

public:
  // Registers
  std::uint8_t A, B, C, D, E, H, L, F;
  void loadOpcodes();
  CPU(BUS& _bus); // aqui necesitmaos pasar por referencia el BUS de memoria
  ~CPU();
  void cpuCycle();
  // LOGS CPU
  void showCPUINFO();
};
#endif // CPU_H
