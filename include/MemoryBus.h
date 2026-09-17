#ifndef BUS_H
#define BUS_H
#include <cstdint>

class BUS {
private:
  std::uint8_t wram[8192];
  std::uint8_t vram[8192];
  std::uint8_t oam[160];
  std::uint8_t hram[127];

public:
  std::uint8_t read(std::uint16_t);
  void write(std::uint16_t);
};
#endif // !BUS_H
