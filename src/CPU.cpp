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
    opcode_table[i] = &CPU::b_illegal_opcode;
  }
    loadOpcodes();
};

CPU::~CPU() {};


const CPU::Reg8Ptr CPU::mapa_registros[8] = {
    &CPU::B, &CPU::C, &CPU::D, &CPU::E, &CPU::H, &CPU::L, nullptr, &CPU::A
};

void CPU::cpuCycle() {
  // 1. FETCH (Búsqueda) - Esto SÍ lo está haciendo bien
  current_opcode = bus.read(PC);
  PC++; 
  
  std::cout << "current_opcode: 0x" 
          << std::hex
          << std::uppercase
          << std::setw(2)
          << std::setfill('0')
          << static_cast<int>(current_opcode)
          << std::endl;

  // 2. DECODE (Decodificación)
  InstructionFunc funcion = opcode_table[current_opcode];
  
  // 3. EXECUTE (Ejecución)
  MCycles += (this->*funcion)();  
}

void CPU::initCpu() {
  A = 0x01, B = 0x00, C = 0x13, D = 0x00, E = 0xD8, H = 0x01, L = 0x4D, F = 0xB0;
  PC = 0x0100;
  SP = 0xFFFE;
  current_opcode = 0;
  MCycles = 0;
}

void CPU::loadOpcodes() {
  opcode_table[0x00] = &CPU::b_nop;
  // opcode_table[0x10] = &CPU::b_stop;
  OpcodeLoaders::load_ld_block(*this);
  // opcode_table[0x76] = &CPU::b_halt;
  OpcodeLoaders::load_INC_block(*this);
  OpcodeLoaders::load_DEC_block(*this);
  OpcodeLoaders::load_ADD_block(*this);
  OpcodeLoaders::load_SUB_block(*this);
  OpcodeLoaders::load_LOGICAL_block(*this);
  OpcodeLoaders::load_LD16BITS_block(*this);
  OpcodeLoaders::load_ADD16BITS_block(*this);
}

int CPU::b_illegal_opcode() {
  std::cerr << "Error: !Opcode no implementado¡\n"
            << "Actual Opcode: " << std::hex << static_cast<int>(current_opcode)
            << "\n"
            << "Program Counter (PC): " << std::hex << PC << "\n";
  std::exit(1);
  return 0;
}

int CPU::b_nop() { return 1; }

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
