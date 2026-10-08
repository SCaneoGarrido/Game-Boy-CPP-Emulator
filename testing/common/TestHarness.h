#ifndef TEST_HARNESS_H
#define TEST_HARNESS_H
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

// =============================================================================
// ARNES DE PRUEBAS PASS/FAIL
// -----------------------------------------------------------------------------
// Cada suite (testing/<suite>/test_<suite>.cpp) define:
//   - code:  bytes del programa, se copian en 0x0100
//   - data:  bytes de datos, se copian en 0x0180 (dentro de la ROM)
//   - steps: un Step por instruccion, con el estado ESPERADO despues de ejecutarla
// runSuite ejecuta una instruccion por Step y compara solo los campos indicados.
// Estado inicial (Start):
//   Start::Zero     -> A..L = 00, F = 00, SP = 0000, PC = 0100 (por defecto).
//                      Precondicion fija: las suites de opcodes no dependen del boot.
//   Start::PostBoot -> el estado que deja CPU::initCpu() (salto de la boot ROM).
// La memoria empieza a 0 en ambos casos.
// =============================================================================

enum class Start { Zero, PostBoot };

// Estado esperado tras una instruccion. Solo se comprueban los campos fijados.
// Uso: E().a(0x10).f(0x20).pc(0x0106).hl(0x0180).mem(0xC000, 0x0F)
struct Expect {
  std::optional<std::uint8_t> A, F, B, C, D, E, H, L;
  std::optional<std::uint16_t> PC, SP;
  std::vector<std::pair<std::uint16_t, std::uint8_t>> memory;

  Expect &a(std::uint8_t v) { A = v; return *this; }
  Expect &f(std::uint8_t v) { F = v; return *this; }
  Expect &b(std::uint8_t v) { B = v; return *this; }
  Expect &c(std::uint8_t v) { C = v; return *this; }
  Expect &d(std::uint8_t v) { D = v; return *this; }
  Expect &e(std::uint8_t v) { E = v; return *this; }
  Expect &h(std::uint8_t v) { H = v; return *this; }
  Expect &l(std::uint8_t v) { L = v; return *this; }
  Expect &pc(std::uint16_t v) { PC = v; return *this; }
  Expect &sp(std::uint16_t v) { SP = v; return *this; }
  Expect &bc(std::uint16_t v) { B = v >> 8; C = v & 0xFF; return *this; }
  Expect &de(std::uint16_t v) { D = v >> 8; E = v & 0xFF; return *this; }
  Expect &hl(std::uint16_t v) { H = v >> 8; L = v & 0xFF; return *this; }
  Expect &mem(std::uint16_t addr, std::uint8_t v) {
    memory.emplace_back(addr, v);
    return *this;
  }
};

inline Expect E() { return Expect{}; }

struct Step {
  std::string desc; // "ADD A,C   0F+01" -> se imprime junto al numero de paso
  Expect exp;
};

// Ejecuta la suite y devuelve 0 si todos los pasos pasan, 1 si alguno falla.
// verbose = true muestra la traza de la CPU/BUS y showCPUINFO() en cada paso.
int runSuite(const std::string &name, const std::vector<std::uint8_t> &code,
             const std::vector<std::uint8_t> &data,
             const std::vector<Step> &steps, bool verbose,
             Start start = Start::Zero);

// Atajo para main(): detecta "-v" en los argumentos.
int runSuiteMain(int argc, char *argv[], const std::string &name,
                 const std::vector<std::uint8_t> &code,
                 const std::vector<std::uint8_t> &data,
                 const std::vector<Step> &steps, Start start = Start::Zero);

#endif // TEST_HARNESS_H
