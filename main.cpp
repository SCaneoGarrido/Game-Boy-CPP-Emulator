#include "./include/CPU.h"
#include "./include/MemoryBus.h"
#include <cstdint>
#include <vector>

int main () {

  std::vector<std::uint8_t> dummyBytes = {0x00, 0x11, 0x22, 0x33};
  BUS bus;
  CPU cpu(bus); 
  bus.load_rom(dummyBytes);

  for (int i = 0; i <= 3; i++) {
     cpu.cpuCycle();
  }
  return 0;
}
