#include "./include/CPU.h"
#include "./include/FileUtils.h"
#include "./include/MemoryBus.h"
#include <cstdlib>
#include <iostream>

// =============================================================================
// Punto de entrada del emulador DMG.
//   ./build/DMGE                    -> estado inicial de la CPU (post-boot)
//   ./build/DMGE <rom.gb> [ciclos]  -> carga la ROM y ejecuta N ciclos (32 por
//                                      defecto) mostrando el estado en cada uno
// Un opcode sin implementar detiene el programa (b_illegal_opcode).
// Las pruebas de opcodes viven en testing/ (una suite por familia, PASS/FAIL):
//   cmake -S . -B build && cmake --build build
//   ctest --test-dir build --output-on-failure
//   ./build/testing/test_<suite> -v      -> una suite con traza completa
// =============================================================================
static const int DEFAULT_CYCLES = 32;

int main(int argc, char *argv[]) {
  BUS bus;
  CPU cpu(bus);

  if (argc < 2) {
    std::cout << "DMGE - estado inicial de la CPU\n";
    cpu.showCPUINFO();
    std::cout << "Uso: DMGE <rom.gb> [ciclos]\n"
              << "Pruebas de opcodes: ctest --test-dir build --output-on-failure\n";
    return 0;
  }

  std::vector<std::uint8_t> rom = read_binary_file(argv[1]);
  if (rom.empty()) {
    std::cerr << "No se pudo cargar la ROM: " << argv[1] << "\n";
    return 1;
  }
  if (rom.size() > 0x8000)
    std::cout << "Aviso: ROM de " << rom.size()
              << " bytes; sin MBC solo se mapean los primeros 32 KB\n";

  int cycles = DEFAULT_CYCLES;
  if (argc >= 3) {
    char *end = nullptr;
    long value = std::strtol(argv[2], &end, 10);
    if (*end != '\0' || value <= 0) {
      std::cerr << "Numero de ciclos invalido: " << argv[2] << "\n";
      return 1;
    }
    cycles = static_cast<int>(value);
  }

  bus.load_rom(rom);
  std::cout << "DMGE - ROM " << argv[1] << " (" << std::dec << rom.size()
            << " bytes), " << cycles << " ciclos\n";
  cpu.showCPUINFO();
  for (int i = 0; i < cycles; i++) {
    cpu.cpuCycle();
    cpu.showCPUINFO();
  }
  return 0;
}
