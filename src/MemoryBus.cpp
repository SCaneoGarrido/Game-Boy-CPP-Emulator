#include "../include/MemoryBus.h"
#include <cstdint>
#include <cstring>
#include <iostream>

BUS::BUS() {};

BUS::~BUS() {};

void BUS::initMemoryBus() {
  IE = 0;
  reg_ff0 = 0x00;
  bios_mapped = true;
}

void BUS::clearArrays() {
  // todos los arrays de memoria en 0
  std::memset(wram, 0, sizeof(wram));
  std::memset(vram, 0, sizeof(vram));
  std::memset(oam, 0, sizeof(oam));
  std::memset(hram, 0, sizeof(hram));
  std::memset(boot_rom, 0, sizeof(boot_rom));
  std::memset(io_registers, 0, sizeof(io_registers));
}

void BUS::load_rom(std::vector<std::uint8_t> bytesinformation) {
  // Carga de ROM temporal para juegos pequeños
  // 32KB maximo
  cartrigbe_rom_bank = bytesinformation;
}

std::uint8_t BUS::read(std::uint16_t address) {
  // Debe ir verificando los rangos de memoria.
  
  if (address <= 0x00FF && bios_mapped) {
    return boot_rom[address];
  }

  if (address <= 0x7FFF) {
    if (address < cartrigbe_rom_bank.size()) {
      return cartrigbe_rom_bank[address];
    }
    return 0xFF;
  }

  else if (address >= 0x8000 && address <= 0x9FFF) {
    return vram[address - 0x8000];
  }
  // de momento no hay EXTERNAL RAM (EN IMPLEMENTACION DEL MBC IRA)
  else if (address >= 0xA000 && address <= 0xBFFF) {
    return 0x00;
  } 
  else if (address >= 0xC000 && address <= 0xDFFF) {
    return wram[address - 0xC000];
  } 
  else if (address >= 0xE000 && address <= 0xFDFF) {
    return wram[(address - 0x2000) - 0xC000];
  }

  else if (address >= 0xFE00 && address <= 0xFEFF) {
    return oam[address - 0xFE00];
  } 
  else if (address >= 0xFF00 && address <= 0xFF7F) {
    return io_registers[address - 0xFE00];
  }

  else if (address >= 0xFF80 && address <= 0xFFFE) {
    return hram[address - 0xFF80];
  }

  else if (address == 0xFFFF) {
    return IE;
  }
  return 0xFF;
}

void BUS::write(std::uint16_t address, std::uint8_t value) {
  std::cout << "Writing in: " << std::hex << address << std::endl;
  std::cout << "Value: " << std::hex << value << std::endl;

  // Terminar la logica de escritura de este metodo.
}
