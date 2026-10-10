#include "../../include/CPU.h"
#include <cstdint>

int CPU::op_DAA() {
  std::uint8_t original_A = A;
  if (!getFlag(FLAG_N)) {
    if (getFlag(FLAG_C) || original_A > 0x99) { 
      A += 0x60;
      setFlag(FLAG_C);
    }
    if (getFlag(FLAG_H) || (original_A & 0x0F) > 0x09) 
      A += 0x6;
  } else {
    if (getFlag(FLAG_C))
      A -= 0x60;
    if (getFlag(FLAG_H))
      A -= 0x6;
  }
  return 1;
}

int CPU::op_CPL() {
  A = ~A;
  setFlag(FLAG_N);
  setFlag(FLAG_H);
  return 1;

}

int CPU::op_CCF() {
  if (getFlag(FLAG_C))
    clearFlag(FLAG_C);

  if (!getFlag(FLAG_C))
    setFlag(FLAG_C);

  return 1;
}

int CPU::op_SCF() {
  setFlag(FLAG_C);
  clearFlag(FLAG_N);
  clearFlag(FLAG_H);
  return 1;
}

int CPU::op_nop() {
  return 1;
}


int CPU::op_halt() {
    std::uint8_t ie = bus.read(0xFFFF);
    std::uint8_t if_reg = bus.read(0xFF0F);
    bool hay_interrupcion_pendiente = (ie & if_reg & 0x1F) > 0;

    if (IME == false && hay_interrupcion_pendiente) {
        // Escenario 4: Gatillamos el HALT Bug
        halt_bug_active = true;
    } else if (IME == true && hay_interrupcion_pendiente) {
        // Escenario 3: No se pausa, se despierta al instante para el siguiente ciclo
        is_halted = false;
    } else {
        // Escenario 1 y 2: Pausa real de la CPU
        is_halted = true;
    }

    return 1; // Ejecutar la instrucción HALT cuesta 1 M-Cycle
}

int CPU::op_ei() {
    pending_ime_enable = true; // Activamos el flag para que se encienda al final del ciclo
    return 1;                  // Cuesta 1 M-Cycle
}

int CPU::op_di() {
    IME = false;               // Apagado inmediato del interruptor maestro
    pending_ime_enable = false; 
    return 1;                  // Cuesta 1 M-Cycle
}

int CPU::op_stop() {
    // 1. Leer el byte fantasma obligatorio (debería ser 0x00) e incrementar PC
    std::uint8_t byte_fantasma = bus.read(PC);
    PC++; 

    // 2. Activar el estado de apagado extremo
    is_stopped = true;

    // Ejecutar STOP en sí toma 1 ciclo de máquina
    return 1; 
}
