#include "../include/CPU.h"
#include "../include/OpcodeLoaders.h"
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <ios>
#include <iostream>


CPU::CPU(BUS &_bus) : bus(_bus) {
  initCpu();
  for (int i = 0; i <= 255; i++) {
    opcode_table[i]    = &CPU::b_illegal_opcode;
    opcode_table_cb[i] = &CPU::b_illegal_opcode;
  }
    loadOpcodes();
};

CPU::~CPU() {};


const CPU::Reg8Ptr CPU::mapa_registros[8] = {
    &CPU::B, &CPU::C, &CPU::D, &CPU::E, &CPU::H, &CPU::L, nullptr, &CPU::A
};


void CPU::cpuCycle() {
    // Leemos los registros del bus para calcular las interrupciones pendientes
    std::uint8_t ie = bus.read(0xFFFF);
    std::uint8_t if_reg = bus.read(0xFF0F);
    std::uint8_t condition_halt = (ie & if_reg & 0x1F);

   
    if (condition_halt > 0) {
        is_halted = false; // Cualquier interrupción pendiente despierta a la CPU del HALT

        if (IME) {
            IME = false; // Deshabilitamos interrupciones globales inmediatamente
            push16(PC);  // Guardamos el punto de retorno usando nuestro helper genérico

            // Evaluamos prioridades bit por bit (del Bit 0 al Bit 4)
            if (condition_halt & 0x01) {        // 1. V-Blank
                if_reg &= ~0x01; // Limpiamos el bit atendido
                PC = 0x0040;     // Saltamos al vector
            } else if (condition_halt & 0x02) { // 2. LCD STAT
                if_reg &= ~0x02;
                PC = 0x0048;
            } else if (condition_halt & 0x04) { // 3. Timer
                if_reg &= ~0x04;
                PC = 0x0050;
            } else if (condition_halt & 0x08) { // 4. Serial
                if_reg &= ~0x08;
                PC = 0x0058;
            } else if (condition_halt & 0x10) { // 5. Joypad
                if_reg &= ~0x10;
                PC = 0x0060;
            }

            bus.write(0xFF0F, if_reg); // Actualizamos el bus con el bit de interrupción limpio
            MCycles += 5;              // Atender la interrupción toma 5 ciclos de máquina
            return;                    // Abortamos el ciclo. No se hace FETCH de la ROM.
        }
    }

    if (is_halted) {
        MCycles += 1; // Consumimos 1 ciclo de máquina esperando
        return;       // Abortamos el ciclo. El PC se queda congelado.
    }
    
    if (is_stopped) {
      MCycles += 1;
      return;
    }
   
    // FETCH (Búsqueda)
    current_opcode = bus.read(PC);
    
    if (halt_bug_active) {
        // Si el bug está activo, NO incrementamos el PC (se lee el mismo byte dos veces)
        halt_bug_active = false; 
    } else {
        PC++; // Flujo normal de avance
    }

    // DECODE (Decodificación)
    InstructionFunc funcion = opcode_table[current_opcode];

    // EXECUTE (Ejecución) y acumulación de ciclos
    MCycles += (this->*funcion)();

    // EFECTO COLATERAL DE EI (Retraso diferido de 1 ciclo)
    if (pending_ime_enable) {
        IME = true;
        pending_ime_enable = false;
    }
}


void CPU::initCpu() {
  // ME ESTOY SALTANDO LA BOOT ROOM
  A = 0x01, B = 0x00, C = 0x13, D = 0x00, E = 0xD8, H = 0x01, L = 0x4D, F = 0xB0;
  PC = 0x0100;
  SP = 0xFFFE;
  IME = false;
  is_halted = false;
  current_opcode = 0;
  MCycles = 0;
}

int CPU::op_cb_prefix_handler() {
  std::uint8_t cb_opcode = bus.read(PC);
  PC++;
    
    std::cout << "  -> CB Opcode: 0x" 
              << std::hex << std::uppercase << std::setw(2) << std::setfill('0') 
              << static_cast<int>(cb_opcode) << std::endl;


    InstructionFunc funcion_cb = opcode_table_cb[cb_opcode];
    
    return (this->*funcion_cb)();
}
void CPU::push16(std::uint16_t value) {
  SP--;
  bus.write(SP, (value >> 8) & 0xFF);
  SP--;
  bus.write(SP, value & 0xFF);
}

std::uint16_t CPU::pop16() {
  std::uint8_t low = bus.read(SP);
  SP++;
  std::uint8_t high = bus.read(SP);
  SP++;
  return (high << 8) | low;
}

void CPU::loadOpcodes() {
  // Asigno el prefijo CB a la tabla principal
  opcode_table[0xCB] = &CPU::op_cb_prefix_handler;
  opcode_table[0x00] = &CPU::op_nop;
  opcode_table[0x10] = &CPU::op_stop;
  opcode_table[0x76] = &CPU::op_halt;
  
  OpcodeLoaders::load_ld_block(*this);
  OpcodeLoaders::load_INC_block(*this);
  OpcodeLoaders::load_DEC_block(*this);
  OpcodeLoaders::load_ADD_block(*this);
  OpcodeLoaders::load_SUB_block(*this);
  OpcodeLoaders::load_LOGICAL_block(*this);
  OpcodeLoaders::load_LD16BITS_block(*this);
  OpcodeLoaders::load_ADD16BITS_block(*this);
  
  // CB Prefix Opcodes 
  OpcodeLoaders::load_SWAP_n_cb_block(*this);
  OpcodeLoaders::load_MISSCELLANEOUS_block(*this);

}

int CPU::b_illegal_opcode() {
  std::cerr << "Error: !Opcode no implementado¡\n"
            << "Actual Opcode: " << std::hex << static_cast<int>(current_opcode)
            << "\n"
            << "Program Counter (PC): " << std::hex << PC << "\n";
  std::exit(1);
  return 0;
}

std::uint16_t CPU::getPairedRegisters(std::uint8_t x_value,
                                     std::uint8_t y_value) {
  std::uint16_t PairedRegister = (x_value << 8) | y_value;
  return PairedRegister;
}

void CPU::setPairedRegisters(std::uint8_t &x_reg, std::uint8_t &y_reg,
                             std::uint16_t value) {
  x_reg = (value >> 8) & 0xFF; // Saco 8 bit superiores
  y_reg = value & 0xFF;        // Saco 8 bits inferiores
}

void CPU::setFlag(std::uint8_t mask) {
  F = F | mask;
  F = F & 0XF0; // los ultimos 4 bits son 0
}

void CPU::clearFlag(std::uint8_t mask) {
  std::uint8_t reversed_mask = ~mask;
  F = F & reversed_mask;
  F = F & 0xF0;
}

bool CPU::getFlag(std::uint8_t mask) { return (F & mask) != 0; }

// Validaciones de flags de 8 BIT
bool CPU::checkHalfCarryAdd(std::uint8_t a, std::uint8_t b,
                            std::uint8_t carry) {
  return ((a & 0x0F) + (b & 0x0F) + carry) > 0x0F;
}

bool CPU::checkCarryAdd(std::uint16_t a, std::uint16_t b, std::uint16_t carry) {
  return (a + b + carry) > 0xFF;
}

bool CPU::checkHalfCarrySub(std::uint8_t a, std::uint8_t b,
                            std::uint8_t carry) {
  return ((a & 0x0F) - (b & 0x0F) - carry) < 0;
}

bool CPU::checkCarrySub(std::uint16_t a, std::uint16_t b, std::uint16_t carry) {
  return a < (b + carry);
}
// ============================================================================
// Validaciones de banderas en 16 bits
bool CPU::check16bitCarryAdd(std::uint16_t a, std::uint16_t b) {
  std::uint32_t result = a + b;
  return result > 0xFFFF;
}

bool CPU::check16bitHalfCarryAdd(std::uint16_t a, std::uint16_t b) {
   return ((a & 0x0FFF) + (b & 0x0FFF)) > 0x0FFF;
}


void CPU::showCPUINFO() {
  bool opcode_is_loaded =
      opcode_table[current_opcode] != &CPU::b_illegal_opcode;

  // Guardamos el estado actual de los flags de formato de cout para no arruinar
  // impresiones externas
  std::ios_base::fmtflags f(std::cout.flags());

  std::cout << std::hex << std::setfill('0') << std::uppercase;

  // 1. Estado de los Registros Principales (PC, SP, y de 8 bits)
  std::cout << "PC:" << std::setw(4) << PC << " SP:" << std::setw(4) << SP
            << " A:" << std::setw(2) << static_cast<int>(A)
            << " F:" << std::setw(2) << static_cast<int>(F)
            << " B:" << std::setw(2) << static_cast<int>(B)
            << " C:" << std::setw(2) << static_cast<int>(C)
            << " D:" << std::setw(2) << static_cast<int>(D)
            << " E:" << std::setw(2) << static_cast<int>(E)
            << " H:" << std::setw(2) << static_cast<int>(H)
            << " L:" << std::setw(2) << static_cast<int>(L);

  // 2. Estado de los Flags individuales (en binario/texto rápido)
  std::cout << " | Flags: " << (getFlag(FLAG_Z) ? 'Z' : '-')
            << (getFlag(FLAG_N) ? 'N' : '-') << (getFlag(FLAG_H) ? 'H' : '-')
            << (getFlag(FLAG_C) ? 'C' : '-');

  // 3. Verificación del Opcode ejecutado
  std::cout << " | Op:0x" << std::setw(2) << static_cast<int>(current_opcode);
  if (!opcode_is_loaded) {
    std::cout << " [ILLEGAL!]";
  } else {
    std::cout << " [OK]";
  }

  std::cout << "\n";

  // Restauramos los flags originales de cout
  std::cout.flags(f);
}
