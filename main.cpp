#include "./include/CPU.h"
#include "./include/MemoryBus.h"
#include <cstdint>
#include <vector>

int main() {
  // ROM de prueba con datos intercalados y flujo real de instrucciones
  std::vector<std::uint8_t> testInstructions = {
      0x00, // 1. NOP -> Avanza PC a 0101

      // --- FAMILIA: LD r8, imm8 (Opcodes de 2 bytes) ---
      0x06, 0x45, // 2. LD B, 0x45  -> B debe ser 0x45. PC avanza a 0103
      0x0E, 0x99, // 3. LD C, 0x99  -> C debe ser 0x99. PC avanza a 0105
      0x16, 0x1A, // 4. LD D, 0x1A  -> D debe ser 0x1A. PC avanza a 0107
      0x1E, 0x2B, // 5. LD E, 0x2B  -> E debe ser 0x2B. PC avanza a 0109
      0x26, 0xC0, // 6. LD H, 0xC0  -> H debe ser 0xC0. PC avanza a 010B
      0x2E, 0xDE, // 7. LD L, 0xDE  -> L debe ser 0xDE. PC avanza a 010D
      0x3E, 0xFF, // 8. LD A, 0xFF  -> A debe ser 0xFF. PC avanza a 010F

      // --- FAMILIA: LD r8, r8 (Opcodes de 1 byte del cargador) ---
      0x78, // 9.  LD A, B    -> A toma el valor de B (Debería pasar de 0xFF a
            // 0x45)
      0x51  // 10. LD D, C    -> D toma el valor de C (Debería pasar de 0x1A a
            // 0x99)
  };

  std::vector<std::uint8_t> testRom(0x0100, 0x00);
  testRom.insert(testRom.end(), testInstructions.begin(),
                 testInstructions.end());

  BUS bus;
  CPU cpu(bus);
  bus.load_rom(testRom);

  // Ejecutamos exactamente las 10 instrucciones mapeadas arriba
  int numero_de_instrucciones = 10;
  for (int i = 0; i < numero_de_instrucciones; i++) {
    cpu.cpuCycle();
    cpu.showCPUINFO();
  }

  return 0;
}
