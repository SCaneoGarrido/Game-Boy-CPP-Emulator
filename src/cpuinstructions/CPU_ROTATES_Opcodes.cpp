#include "../../include/CPU.h"
#include <cstdint>

// ===================================== ROTATES =================================
int CPU::op_RLCA() {
  std::uint8_t bitout7 = (A >> 7) & 1;
  // El '& 0xFF' es una máscara obligatoria para eliminar los bits excedentes 
  // que C++ genera en los bits altos debido a la "promoción de enteros" a 32 bits,
  // garantizando que el registro se mantenga estrictamente en 8 bits.

  A = (A << 1) & 0xFF; // Máscara contra promoción a 32 bits de C++; asegura 8 bits.
  A = A | bitout7;
  
  clearFlag(FLAG_Z);
  clearFlag(FLAG_H);
  clearFlag(FLAG_N);
  
  if (bitout7 == 1 ) 
    setFlag(FLAG_C);

  if (bitout7 == 0)
    clearFlag(FLAG_C);
  
  return 1;
}

int CPU::op_RLA() {
  // Hacemos una rotacion de bits a la izquierda
  // el bit numero 7 pasa a C
  // y el valor de C pasa al bit0
  std::uint8_t bitout7 = (A >> 7) & 1;
  std::uint8_t actual_CFlag_value = getFlag(FLAG_C) ? 1 : 0;
  
  // Muevo el byte un bit a la izquierda y aplico mascara de proteccion
  A  = (A << 1) & 0xFF;

  A = A | actual_CFlag_value;
  if (bitout7 == 1)
    setFlag(FLAG_C);
  if (bitout7 == 0)
    clearFlag(FLAG_C);

  clearFlag(FLAG_Z);
  clearFlag(FLAG_N);
  clearFlag(FLAG_H);

  return 1;
}

int CPU::op_RRCA() {
  // A:        1011 0111
  // bitout0:  0000 0001
  // Shift A:  0101 1011 
  // Result A: 0101 1011

  // Obtengo el bit que se caera cuando desplacemos a la derecha en 1
  std::uint8_t bitout0 = A & 1; // bitout0: 0000 0001 
  
  // Hago el desplazamiento
  A = (A >> 1) & 0xFF; // aqui muevo el registro a la derecha en 1
  // En esta OR movemos el bitout0 (0000 0001) 7 espacioes a la izquierda
  // asi podemos reinsertar lo en el registro de A en el bit bitout7
  // pasamos de 0000 0001 -> 1000 0000 
  A = A | (bitout0 << 7); // 0101 1011 | 1000 0000
  clearFlag(FLAG_Z);
  clearFlag(FLAG_N);
  clearFlag(FLAG_H);

  if (bitout0 == 1)
    setFlag(FLAG_C);
  if (bitout0 == 0)
    clearFlag(FLAG_C);

  return 1;
}

int CPU::op_RRA() {
  // Recupero el bit 0 
  std::uint8_t bitout0 = A & 1; 
  std::uint8_t actual_CFlag_value = getFlag(FLAG_C) ? 1 : 0;
  A = (A >> 1) | (actual_CFlag_value << 7); // NO OLVIDES QUE EL REGISTRO A DEBE HACER EL SHIFT
  // 1010 1101 | 1000 0000
  clearFlag(FLAG_Z);
  clearFlag(FLAG_N);
  clearFlag(FLAG_H);

  if (bitout0 == 1)
    setFlag(FLAG_C);
  if (bitout0 == 0)
    clearFlag(FLAG_C);
  return 1;
}
