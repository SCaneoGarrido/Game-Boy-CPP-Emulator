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


void CPU::cpuCycle() {
  // 1. FETCH (Búsqueda) - Esto SÍ lo está haciendo bien
  current_opcode = bus.read(PC);
  PC++; 
  
  std::cout << "Opcode Obtenido: " << std::hex << static_cast<int>(current_opcode) << std::endl;

  // 2. DECODE (Decodificación)
  InstructionFunc funcion = opcode_table[current_opcode];

  // 3. EXECUTE (Ejecución) - ¡¡ESTA LÍNEA DE ABAJO FALTA O NO SE ESTÁ DISPARANDO!!
  (this->*funcion)(); 
}

void CPU::initCpu() {
  A = 0, B = 0, C = 0, D = 0, E = 0, H = 0, L = 0, F = 0;
  PC = 0x0100;
  SP = 0;
  current_opcode = 0;
}

void CPU::loadOpcodes() {
  // Instruccion NOP
  opcode_table[0x00] = &CPU::b_nop;
  // Instruccion STOP
  // opcode_table[0x10] = &CPU::b_stop;
  OpcodeLoaders::load_ld_8bits_block(*this);
  // opcode_table[0x76] = &CPU::b_halt;
}

void CPU::fetchOpcode() {
  current_opcode = bus.read(PC);
  std::cout << "Opcode Obtenido: " << std::hex
            << static_cast<int>(current_opcode) << std::endl;
  PC++;
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
  return (x_value << 8) | y_value;
}

void CPU::setPairedRegisters(std::uint8_t &x_reg, std::uint8_t &y_reg,
                             std::uint16_t value) {
  x_reg = (value << 8) & 0xFF; // Saco 8 bit superiores
  y_reg = value & 0xFF;        // Saco 8 bits inferiores
}
/*
std::uint16_t CPU::getHlDirection() {
  return (H << 8) | L;
}
*/
void CPU::setFlag(std::uint8_t mask) {
  F = F | mask;
  F = F & 0XF0; // los ultomos 3 bits son 0
}

void CPU::clearFlag(std::uint8_t mask) {
  std::uint8_t reversed_mask = ~mask;
  F = F & reversed_mask;
  F = F & 0xF0;
}

bool CPU::getFlag(std::uint8_t mask) { return (F & mask) != 0; }

// Operaciones de suma
bool CPU::checkHalfCarryAdd(std::uint8_t a, std::uint8_t b,
                            std::uint8_t carry) {
  return ((a & 0x0F) + (b & 0x0F) + carry) > 0x0F;
}

bool CPU::checkCarryAdd(std::uint16_t a, std::uint16_t b, std::uint16_t carry) {
  return (a + b + carry) > 0xFF;
}

// Operaciones de substraccion
bool CPU::checkHalfCarrySub(std::uint8_t a, std::uint8_t b,
                            std::uint8_t carry) {
  return (a & 0x0F) < ((b & 0x0F) + carry);
}

// Devuelve true si el valor a restar es mayor que el que tenemos (resultado <
// 0)
bool CPU::checkCarrySub(std::uint16_t a, std::uint16_t b, std::uint16_t carry) {
  return a < (b + carry);
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
