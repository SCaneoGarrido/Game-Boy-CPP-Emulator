#ifndef BUS_H
#define BUS_H
#include <cstdint>
#include <vector>

class BUS {
private:
  std::uint8_t wram[8192];
  std::uint8_t vram[8192];
  std::uint8_t oam[160];
  std::uint8_t hram[127];
  std::uint8_t io_registers[128];
  std::vector<std::uint8_t> cartrigbe_rom_bank; // Aqui cargo el juego
  std::uint8_t IE;
  // Componentes de la boot room
  std::uint8_t boot_rom[256];
  bool bios_mapped;
  std::uint8_t reg_ff0;
  void clearArrays(); 
public:
  BUS();
  ~BUS();

  void load_rom(std::vector<std::uint8_t> bytesinformation);
  void load_boot_rom(std::vector<std::uint8_t> boot_bytes);

  std::uint8_t read(std::uint16_t address);
  void write(std::uint16_t address, std::uint8_t value); // no implementado

  void initMemoryBus();
};
#endif // !BUS_H
