#ifndef OPCODELOADERS_H
#define OPCODELOADERS_H
class CPU;
namespace OpcodeLoaders {
  // Opcodes de tabla General
  void load_ld_block(CPU& cpu);
  void load_INC_block(CPU& cpu);
  void load_DEC_block(CPU& cpu);
  void load_ADD_block(CPU& cpu);
  void load_SUB_block(CPU& cpu);
  void load_LOGICAL_block(CPU& cpu);
  void load_LD16BITS_block(CPU& cpu);
  void load_ADD16BITS_block(CPU& cpu);

  // Opcodes de prefijo CB
  void load_SWAP_n_cb_block(CPU& cpu);
  
  // Opcodes Miscellaneous
  void load_MISSCELLANEOUS_block(CPU& cpu);
}


#endif // !OPCODELOADERS_H


