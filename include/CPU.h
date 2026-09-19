#ifndef CPU_H
#define CPU_H
#include "../include/MemoryBus.h"
#include <cstdint>
#include <string>

class CPU {
private:
  // Registers
  std::uint8_t A, B, C, D, E, H, L, F;
  // Flags
  //std::uint8_t z, n, h, c;
  // Special Registers
  std::uint16_t PC, SP;
  // opcode
  std::uint8_t opcode;
  // Flags masks
  const std::uint8_t FLAG_Z = 0x80;
  const std::uint8_t FLAG_N = 0x40;
  const std::uint8_t FLAG_H = 0x20;
  const std::uint8_t FLAG_C = 0x10;
  // MemoryBus
  BUS& bus;

  void initCpu();
  void clearResources();
  void fetchOpcode(); // Busca la instruccion almadenada en la direccion de PC
  void decodeExecute();
  using InstructionFunc = int (CPU::*)();
  int b_illegal_opcode();
  int b_nop();
  InstructionFunc opcode_table[256];
  
  void setFlag(std::uint8_t mask);
  void clearFlag(std::uint8_t mask);
  bool getFlag(std::uint8_t mask);
  
  bool checkHalfCarryAdd(std::uint8_t a, std::uint8_t b, std::uint8_t carry);
  bool checkCarryAdd(std::uint16_t a, std::uint16_t b, std::uint16_t carry);
  
  bool checkHalfCarrySub(std::uint8_t a, std::uint8_t b, std::uint8_t carry);
  bool checkCarrySub(std::uint16_t a, std::uint16_t b, std::uint16_t carry);

public:
  CPU(BUS& _bus); // aqui necesitmaos pasar por referencia el BUS de memoria
  ~CPU();
  void cpuCycle();
};

#endif // CPU_H
