#include "../include/MemoryBus.h"
#include <cstdint>
#include <cstring>
#include <iostream>

BUS::BUS() {

  initMemoryBus();
};

BUS::~BUS() {};

void BUS::initMemoryBus() {
  IE = 0;
  reg_ff0 = 0x00;
  bios_mapped = true;
  clearArrays();
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
    return io_registers[address - 0xFF00];
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
  std::cout << "Writing in: " << std::hex << static_cast<int>(address) << std::endl;
  std::cout << "Value: " << std::hex << static_cast<int>(value) << std::endl;
 
  // 1. ROM del Cartucho (SÓLO LECTURA - Bloqueamos la escritura por ahora)
  if (address >= 0x0000 && address <= 0x7FFF) {
    // Los juegos escriben aquí para controlar los MBC (bancos), déjalo vacío de momento.
    return;
  }
  
  // 2. VRAM (Memoria de vídeo)
  else if (address >= 0x8000 && address <= 0x9FFF) {
    vram[address - 0x8000] = value;
  }
  
  // 3. WRAM
  // Cubre el rango normal (0xC000-0xDFFF) y su espejo Echo RAM (0xE000-0xFDFF)
  else if (address >= 0xC000 && address <= 0xFDFF) {
    // Usamos una máscara 0x1FFF (8191 decimal) para que tanto la WRAM como su espejo 
    // apunten perfectamente dentro de tu arreglo de 8192 bytes.
    wram[address & 0x1FFF] = value;
  }
  
  // 4. OAM (Object Attribute Memory - Atributos de Sprites)
  else if (address >= 0xFE00 && address <= 0xFE9F) {
    oam[address - 0xFE00] = value;
  }
  
  // 5. Zona no permitida / Prohibida en hardware real (0xFEA0 - 0xFEFF)
  else if (address >= 0xFEA0 && address <= 0xFEFF) {
    return; 
  }
  
  // 6. Registros de I/O (Joypad, Timers, Sonido...)
  else if (address >= 0xFF00 && address <= 0xFF7F) {
    if (address == 0xFF50) {
      // Registro especial para apagar la BIOS/Boot ROM
      bios_mapped = false; 
    }
    io_registers[address - 0xFF00] = value;
  }
  
  // 7. HRAM (High RAM - Memoria rápida de la pila)
  else if (address >= 0xFF80 && address <= 0xFFFE) {
    hram[address - 0xFF80] = value;
  }
  
  // 8. IE (Interrupt Enable Register)
  else if (address == 0xFFFF) {
    IE = value;
  }
}
