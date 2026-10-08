#include "./include/CPU.h"
#include "./include/MemoryBus.h"
#include <iostream>

// =============================================================================
// Punto de entrada del emulador DMG.
// Las pruebas de opcodes viven en testing/ (una suite por familia, PASS/FAIL):
//   cmake -S . -B build && cmake --build build
//   ctest --test-dir build --output-on-failure
//   ./build/testing/test_<suite> -v      -> una suite con traza completa
// =============================================================================
int main() {
  BUS bus;
  CPU cpu(bus);

  std::cout << "DMGE - estado inicial de la CPU\n";
  cpu.showCPUINFO();
  std::cout << "Pruebas de opcodes: ctest --test-dir build --output-on-failure\n";
  return 0;
}
