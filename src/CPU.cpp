#include "../include/CPU.h"
#include <cstdint>
#include <cstdlib>
#include <ios>
#include <iostream>

CPU::CPU(BUS &_bus) : bus(_bus) {
  initCpu();
  for (int i = 0; i <= 255; i++) {
    opcode_table[i] = &CPU::b_illegal_opcode;
  }

  opcode_table[0x00] = &CPU::b_nop;
};

CPU::~CPU() {};

void CPU::cpuCycle() {
  fetchOpcode();

  // decodeExecute();
}

void CPU::initCpu() {
  A = 0, B = 0, C = 0, D = 0, E = 0, H = 0, L = 0, F = 0;

  PC = 0x0100;
  SP = 0;
  opcode = 0;
}

void CPU::fetchOpcode() {
  opcode = bus.read(PC);
  std::cout << "Opcode Obtenido: " << std::hex << static_cast<int>(opcode)
            << std::endl;
  PC++;
}

int CPU::b_illegal_opcode() {
  std::cerr << "Error: !Opcode no implementado¡\n"
            << "Actual Opcode: " << std::hex << static_cast<int>(opcode) << "\n"
            << "Program Counter (PC): " << std::hex << PC << "\n";
  std::exit(1);
  return 0;
}

int CPU::b_nop() { return 1; }

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
