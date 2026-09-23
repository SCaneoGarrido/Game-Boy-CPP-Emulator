#ifndef OPCODELOADERS_H
#define OPCODELOADERS_H
class CPU;
namespace OpcodeLoaders {
  // Definicion de la Definicion
  void load_ld_8bits_block(CPU& cpu);
  void load_ld_INC_block(CPU& cpu);
}
#endif // !OPCODELOADERS_H


