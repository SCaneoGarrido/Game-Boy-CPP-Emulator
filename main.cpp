#include "./include/CPU.h"
#include "./include/MemoryBus.h"
#include <cstdint>
#include <vector>

int main() {
  // ROM de prueba con la familia LD completa e incrementos intercalados
  std::vector<std::uint8_t> testInstructions = {
      0x00, // 1. NOP -> Avanza PC a 0101 (1 M-Cycle)

      // --- FAMILIA: LD r8, imm8 (Opcodes de 2 bytes) ---
      0x06, 0x45, // 2. LD B, 0x45  -> B = 0x45. PC -> 0103
      0x0E, 0x99, // 3. LD C, 0x99  -> C = 0x99. PC -> 0105
      0x16, 0x1A, // 4. LD D, 0x1A  -> D = 0x1A. PC -> 0107
      0x1E, 0x2B, // 5. LD E, 0x2B  -> E = 0x2B. PC -> 0109
      0x26, 0xC0, // 6. LD H, 0xC0  -> H = 0xC0. PC -> 010B
      0x2E, 0x00, // 7. LD L, 0x00  -> L = 0x00. PC -> 010D (HL ahora apunta a 0xC000)
      0x3E, 0xFF, // 8. LD A, 0xFF  -> A = 0xFF. PC -> 010F

      // --- FAMILIA: LD r8, r8 (Opcodes de 1 byte) ---
      0x78, // 9.  LD A, B    -> A toma el valor de B (De 0xFF pasa a 0x45)
      0x51, // 10. LD D, C    -> D toma el valor de C (De 0x1A pasa a 0x99)

      // --- NUEVOS OPCODES: INC nn (16 bits, NO modifica banderas) ---
      0x03, // 11. INC BC     -> BC pasa de 0x4599 a 0x459A. (2 M-Cycles)
      0x33, // 12. INC SP     -> SP se incrementa en 1 (ej: 0xFFFE -> 0xFFFF). (2 M-Cycles)

      // --- NUEVOS OPCODES: INC r8 (8 bits, SÍ modifica banderas Z, N, H) ---
      0x04, // 13. INC B      -> B pasa de 0x45 a 0x46. (1 M-Cycle)
            //                  Banderas: Z=0, N=0, H=0 (no hay acarreo en bit 3).
      0x3C, // 14. INC A      -> A pasa de 0x45 a 0x46. (1 M-Cycle)

      // --- CASO ESPECIAL: INC (HL) ---
      // Como H=0xC0 y L=0x00, HL apunta a la dirección 0xC000 (WRAM).
      0x34  // 15. INC (HL)   -> Lee RAM[0xC000], le suma 1 y lo guarda ahí mismo. (3 M-Cycles)
            //                  Banderas: Z, N, H se actualizan según el valor de esa RAM.
  };

  std::vector<std::uint8_t> testRom(0x0100, 0x00);
  testRom.insert(testRom.end(), testInstructions.begin(),
                 testInstructions.end());

  BUS bus;
  CPU cpu(bus);
  bus.load_rom(testRom);

  // Ejecutamos exactamente las 10 instrucciones mapeadas arriba
  int numero_de_instrucciones = 10;
  for (int i = 0; i < sizeof(testInstructions); i++) {
    cpu.cpuCycle();
    cpu.showCPUINFO();
  }

  return 0;
}
