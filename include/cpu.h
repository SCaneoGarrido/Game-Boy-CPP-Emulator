#ifndef CPU_H
#define CPU_H
#include <cstdint>
#include <string>

class CPU {
private:
  // Registers
  std::uint8_t A,B,C,D,E,H,L;
  // Flags
  std::uint8_t z,n,h,c;
  // Special Registers
  std::uint16_t PC, SP;
  // opcode 
  std::uint8_t opcode;

public:
  CPU(); // aqui necesitmaos pasar por referencia el BUS de memoria
  ~CPU();
};

#endif // !CPU_H
