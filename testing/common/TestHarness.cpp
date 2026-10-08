#include "TestHarness.h"
#include "CPU.h"
#include "MemoryBus.h"
#include <iomanip>
#include <iostream>
#include <sstream>

// Acceso al estado privado de la CPU (friend en include/CPU.h).
// Lectura para comparar; la unica escritura es zeroRegisters(), que fija la
// precondicion Start::Zero antes del primer paso.
class CPUTestProbe {
public:
  static void zeroRegisters(CPU &c) {
    c.A = c.F = c.B = c.C = c.D = c.E = c.H = c.L = 0x00;
    c.SP = 0x0000;
  }
  static std::uint8_t A(const CPU &c) { return c.A; }
  static std::uint8_t F(const CPU &c) { return c.F; }
  static std::uint8_t B(const CPU &c) { return c.B; }
  static std::uint8_t C(const CPU &c) { return c.C; }
  static std::uint8_t D(const CPU &c) { return c.D; }
  static std::uint8_t E(const CPU &c) { return c.E; }
  static std::uint8_t H(const CPU &c) { return c.H; }
  static std::uint8_t L(const CPU &c) { return c.L; }
  static std::uint16_t PC(const CPU &c) { return c.PC; }
  static std::uint16_t SP(const CPU &c) { return c.SP; }
  static bool isRegistered(const CPU &c, std::uint8_t opcode) {
    return c.opcode_table[opcode] != &CPU::b_illegal_opcode;
  }
};

static const std::uint16_t CODE_ADDR = 0x0100;
static const std::uint16_t DATA_ADDR = 0x0180;
static const std::size_t ROM_SIZE = 0x0200;

static std::string hex(unsigned value, int width) {
  std::ostringstream out;
  out << std::hex << std::uppercase << std::setw(width) << std::setfill('0')
      << value;
  return out.str();
}

// Compara un campo opcional; si no coincide anade el error a la lista
template <typename T>
static void check(std::vector<std::string> &errors, const std::string &field,
                  const std::optional<T> &expected, T actual, int width) {
  if (expected && *expected != actual)
    errors.push_back(field + " esperado " + hex(*expected, width) +
                     ", obtenido " + hex(actual, width));
}

int runSuite(const std::string &name, const std::vector<std::uint8_t> &code,
             const std::vector<std::uint8_t> &data,
             const std::vector<Step> &steps, bool verbose, Start start) {
  std::cout << "==================================================\n"
            << " SUITE " << name << " (" << steps.size() << " pasos, inicio "
            << (start == Start::Zero ? "a cero" : "post-boot") << ")\n"
            << "==================================================\n";

  std::vector<std::uint8_t> rom(ROM_SIZE, 0x00);
  for (std::size_t i = 0; i < code.size(); i++)
    rom[CODE_ADDR + i] = code[i];
  for (std::size_t i = 0; i < data.size(); i++)
    rom[DATA_ADDR + i] = data[i];

  // El constructor de la CPU tambien imprime (carga de opcodes): solo con -v
  std::streambuf *original_out = std::cout.rdbuf();
  std::ostringstream construction_log;
  if (!verbose)
    std::cout.rdbuf(construction_log.rdbuf());
  BUS bus;
  CPU cpu(bus);
  std::cout.rdbuf(original_out);
  bus.load_rom(rom);
  if (start == Start::Zero)
    CPUTestProbe::zeroRegisters(cpu);

  std::size_t passed = 0;
  for (std::size_t i = 0; i < steps.size(); i++) {
    const Step &step = steps[i];
    const Expect &exp = step.exp;
    std::string label = std::to_string(i + 1) + ". " + step.desc;

    // Un opcode sin registrar haria exit(1) dentro de b_illegal_opcode:
    // se detecta antes y se corta la suite con un FAIL legible.
    std::uint8_t opcode = bus.read(CPUTestProbe::PC(cpu));
    if (!CPUTestProbe::isRegistered(cpu, opcode)) {
      std::cout << "[FAIL] " << label << " -> opcode 0x" << hex(opcode, 2)
                << " no registrado en opcode_table; suite detenida\n";
      break;
    }

    // La CPU y el BUS imprimen trazas en cada instruccion: solo con -v.
    // cpuCycle() deja std::hex activo en cout, asi que tambien se restauran
    // los flags de formato (si no, el resumen saldria en hexadecimal).
    std::streambuf *original = std::cout.rdbuf();
    std::ios_base::fmtflags flags = std::cout.flags();
    char fill = std::cout.fill();
    std::ostringstream discard;
    if (!verbose)
      std::cout.rdbuf(discard.rdbuf());
    cpu.cpuCycle();
    if (verbose)
      cpu.showCPUINFO();
    std::cout.rdbuf(original);
    std::cout.flags(flags);
    std::cout.fill(fill);

    std::vector<std::string> errors;
    check(errors, "A", exp.A, CPUTestProbe::A(cpu), 2);
    check(errors, "F", exp.F, CPUTestProbe::F(cpu), 2);
    check(errors, "B", exp.B, CPUTestProbe::B(cpu), 2);
    check(errors, "C", exp.C, CPUTestProbe::C(cpu), 2);
    check(errors, "D", exp.D, CPUTestProbe::D(cpu), 2);
    check(errors, "E", exp.E, CPUTestProbe::E(cpu), 2);
    check(errors, "H", exp.H, CPUTestProbe::H(cpu), 2);
    check(errors, "L", exp.L, CPUTestProbe::L(cpu), 2);
    check(errors, "PC", exp.PC, CPUTestProbe::PC(cpu), 4);
    check(errors, "SP", exp.SP, CPUTestProbe::SP(cpu), 4);
    for (const auto &[addr, value] : exp.memory)
      check(errors, "(" + hex(addr, 4) + ")", std::optional<std::uint8_t>(value),
            bus.read(addr), 2);

    if (errors.empty()) {
      passed++;
      std::cout << "[PASS] " << label << "\n";
    } else {
      std::cout << "[FAIL] " << label << " ->";
      for (std::size_t e = 0; e < errors.size(); e++)
        std::cout << (e ? ";" : "") << " " << errors[e];
      std::cout << "\n";
    }
  }

  bool ok = passed == steps.size();
  std::cout << "--------------------------------------------------\n"
            << " RESULTADO " << name << ": " << passed << "/" << steps.size()
            << (ok ? "  OK" : "  FALLA") << "\n";
  return ok ? 0 : 1;
}

int runSuiteMain(int argc, char *argv[], const std::string &name,
                 const std::vector<std::uint8_t> &code,
                 const std::vector<std::uint8_t> &data,
                 const std::vector<Step> &steps, Start start) {
  bool verbose = false;
  for (int i = 1; i < argc; i++)
    if (std::string(argv[i]) == "-v")
      verbose = true;
  return runSuite(name, code, data, steps, verbose, start);
}
