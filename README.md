# Game Boy C++ Emulator

Emulador experimental de la **Nintendo Game Boy clásica (DMG)**, escrito en C++17 y orientado al aprendizaje de la arquitectura del hardware: ciclo *fetch-decode-execute*, registros de la CPU, tabla de opcodes, banderas, interrupciones y mapa de memoria.

El proyecto se centra hoy en el núcleo de la CPU (LR35902) y el bus de memoria. Cada familia de instrucciones se valida con una suite de pruebas propia (`testing/`), y el ejecutable `DMGE` puede cargar una ROM y ejecutarla ciclo a ciclo mostrando el estado de la CPU.

> **Estado del proyecto:** prototipo en desarrollo. Todavía no ejecuta juegos: faltan saltos, llamadas, retornos, gran parte del prefijo `CB`, PPU, timers y MBC.

## Avance actual

### CPU

- Registros de 8 bits `A F B C D E H L`, pares `AF BC DE HL`, `PC` y `SP`.
- Arranque en el estado post-boot del DMG (`A=01 F=B0 BC=0013 DE=00D8 HL=014D SP=FFFE PC=0100`), saltando la boot ROM.
- Banderas `Z N H C` en el registro `F`; los 4 bits bajos siempre quedan a cero.
- Tabla principal `opcode_table[256]` y tabla del prefijo `opcode_table_cb[256]` con punteros a métodos miembro; los opcodes sin registrar apuntan a `b_illegal_opcode()`. Cada operación CB devuelve el total de M-cycles de la instrucción, prefijo incluido.
- Contador de **M-cycles** acumulado por instrucción.
- Interrupciones: `IME`, despacho por prioridad a los vectores `0040–0060` (5 M-cycles), `HALT` con *HALT bug*, `STOP`, y `EI` con su retraso de una instrucción.

### Instrucciones implementadas

| Familia | Opcodes | Suite |
|---|---|---|
| `NOP` | `00` | `ld`, `misc` |
| `LD r8,d8` / `LD (HL),d8` | `06 0E 16 1E 26 2E 3E` / `36` | `ld`, `mem` |
| `LD r8,r8` / `LD r8,(HL)` / `LD (HL),r8` | `40–7F` (sin `76`) | `ld`, `mem` |
| `LD A,(rr)` / `LD (rr),A` con `BC DE HL+ HL-` | `0A 1A 2A 3A` / `02 12 22 32` | `ld16`, `mem` |
| `LDH` y `LD (C)` / `LD (a16)` | `E0 F0 E2 F2 EA FA` | `ldh` |
| `LD rr,d16`, `LD SP,HL` | `01 11 21 31`, `F9` | `ld16imm` |
| `LD (a16),SP`, `LD HL,SP+e8` | `08`, `F8` | `ldsp` |
| `PUSH rr` / `POP rr` | `C5 D5 E5 F5` / `C1 D1 E1 F1` | `stack` |
| `INC` / `DEC` de 8 y 16 bits | `04…3C 34 03…33` / `05…3D 35 0B…3B` | `inc`, `dec` |
| `ADD` / `ADC A,n` | `80–8F C6 CE` | `add` |
| `SUB` / `SBC A,n` | `90–9F D6 DE` | `sub` |
| `AND` / `OR` / `XOR` / `CP` | `A0–BF E6 F6 EE FE` | `and`, `logic` |
| `ADD HL,rr`, `ADD SP,e8` | `09 19 29 39`, `E8` | `add16` |
| `DAA`, `CPL`, `SCF`, `CCF` | `27 2F 37 3F` | `misc` |
| `DI`, `EI`, `HALT`, `STOP` | `F3 FB 76 10` | `control`, `interrupts` |
| `RLCA`, `RRCA`, `RLA`, `RRA` | `07 0F 17 1F` | `rotates` |
| Prefijo `CB`: `RLC`, `RRC`, `RL`, `RR` | `CB 00–1F` | `cbrot` |
| Prefijo `CB`: `SLA`, `SRA`, `SRL` | `CB 20–2F`, `CB 38–3F` | `cbshift` |
| Prefijo `CB`: `SWAP` | `CB 30–37` | `swap` |

### Bus de memoria

| Rango | Región | Estado |
|---|---|---|
| `0x0000–0x7FFF` | ROM del cartucho | Lectura; las escrituras se ignoran (MBC pendiente) |
| `0x0000–0x00FF` | Boot ROM | Mapeada solo si se carga una; `FF50` la desmapea |
| `0x8000–0x9FFF` | VRAM | Lectura y escritura |
| `0xA000–0xBFFF` | RAM externa | Devuelve `0x00`; MBC pendiente |
| `0xC000–0xDFFF` | WRAM | Lectura y escritura |
| `0xE000–0xFDFF` | Echo RAM | Espejo de WRAM |
| `0xFE00–0xFE9F` | OAM | Lectura y escritura |
| `0xFF00–0xFF7F` | Registros de E/S | Lectura y escritura (sin periféricos detrás) |
| `0xFF0F` | IF | Lectura y escritura (los bits 5–7 se leen a 1) |
| `0xFF80–0xFFFE` | HRAM | Lectura y escritura |
| `0xFFFF` | IE | Lectura y escritura |

`BUS::write()` imprime cada escritura (dirección y valor) como traza de depuración.

## Arquitectura

```text
main.cpp ──► BUS + CPU ──► (opcional) carga una ROM y ejecuta N ciclos

CPU::cpuCycle()
   ├── interrupciones pendientes (IE & IF) → despacho si IME=1, despierta de HALT
   ├── HALT / STOP → consume 1 M-cycle sin hacer fetch
   ├── fetch: BUS::read(PC), PC++ (salvo HALT bug)
   ├── decode: opcode_table[opcode]  (0xCB → opcode_table_cb[siguiente byte])
   ├── execute: la función devuelve sus M-cycles
   └── EI pendiente de la instrucción anterior → IME = 1

BUS
   └── traduce direcciones de 16 bits a ROM, VRAM, WRAM, OAM, E/S, HRAM, IE/IF
```

Los cargadores de `utils/OpcodesLoaders.cpp` rellenan las tablas por familias (`load_ld_block`, `load_ADD_block`, `load_ROTATES_CB_block`, …). Las operaciones genéricas son templates parametrizados por registro (`std::uint8_t CPU::*`) y por modo de direccionamiento (`hl_modified`, `is_imm8`, …), con instanciaciones explícitas en cada archivo de familia.

CMake compila el núcleo como la biblioteca estática `dmg_core`, que comparten el ejecutable `DMGE` y todas las suites de prueba.

## Estructura del repositorio

```text
.
├── CMakeLists.txt                 # dmg_core + DMGE + testing/
├── main.cpp                       # Punto de entrada: estado inicial o ejecución de una ROM
├── include/                       # CPU, BUS, cargadores de opcodes y utilidades
├── src/
│   ├── CPU.cpp                    # Ciclo de CPU, interrupciones, flags, pila
│   ├── MemoryBus.cpp              # Mapa de memoria
│   ├── cpuinstructions/           # LD, LD16, INC, DEC, ADD, SUB, lógicas, ADD16, rotaciones
│   ├── cpuinstructionsCB/         # Prefijo CB: rotaciones, desplazamientos y SWAP
│   └── cpumiscellaneous/          # DAA, CPL, SCF, CCF, NOP, HALT, STOP, DI, EI
├── utils/                         # Registro de opcodes y lectura de archivos binarios
├── testing/
│   ├── common/                    # Arnés de pruebas (TestHarness)
│   └── <suite>/                   # test_<suite>.cpp + checklist_<suite>.txt
└── doc/
    ├── components/                # Análisis de la CPU y del mapa de memoria
    ├── ideas/                     # Notas de diseño y conceptos
    └── opcodes_mapping/           # Checklist global de validación y hallazgos
```

## Requisitos

- Compilador con soporte de C++17.
- CMake `3.16` o posterior.

No hay dependencias externas.

## Compilar y ejecutar

```bash
cmake -S . -B build
cmake --build build

./build/DMGE                      # muestra el estado inicial de la CPU
./build/DMGE rom.gb               # carga la ROM y ejecuta 32 ciclos
./build/DMGE rom.gb 200           # ... o el número de ciclos indicado
```

En cada ciclo se imprime `PC`, `SP`, registros, banderas y el opcode ejecutado. Sin MBC solo se mapean los primeros 32 KB de la ROM, y un opcode no implementado detiene el programa indicando el opcode y el `PC`.

En Windows el ejecutable puede quedar en una subcarpeta del backend de CMake, por ejemplo `build/Debug/DMGE.exe`.

## Pruebas

Cada familia de instrucciones tiene una suite en `testing/<suite>/`:

- `test_<suite>.cpp`: un programa pequeño (bytes en `0x0100`, datos opcionales en `0x0180`) y, por cada instrucción, el estado **esperado** después de ejecutarla: registros, `F`, `PC`, `SP`, memoria, `IME`, `HALT`, `STOP` y M-cycles. Solo se comparan los campos indicados.
- `checklist_<suite>.txt`: la lista de casos con su resultado (`[x]` pasa, `[!]` falla, `[-]` opcode no registrado) y los hallazgos relacionados.

```bash
ctest --test-dir build --output-on-failure    # todas las suites
ctest --test-dir build -R '^misc$'            # una suite
./build/testing/test_misc -v                  # traza completa de CPU/BUS en cada paso
```

Las suites arrancan con los registros a cero (`Start::Zero`) para no depender del estado de arranque; la suite `boot` valida precisamente ese estado post-boot. El resumen de cobertura y la lista de hallazgos están en [`doc/opcodes_mapping/checklist_test_GB_emulator_26092026.txt`](doc/opcodes_mapping/checklist_test_GB_emulator_26092026.txt).

Estado a 14/10/2026: **las 22 suites pasan** (488 pasos).

## Añadir o modificar instrucciones

1. Declara la operación en `include/CPU.h`.
2. Implementa la lógica en el archivo de su familia (`src/cpuinstructions/`, `src/cpuinstructionsCB/` o `src/cpumiscellaneous/`); cualquier `.cpp` nuevo en esas carpetas entra solo en el build.
3. Añade las instanciaciones explícitas de los templates.
4. Registra los opcodes en `utils/OpcodesLoaders.cpp` y llama al cargador desde `CPU::loadOpcodes()` (declarándolo `friend` en `CPU.h`).
5. Crea `testing/<suite>/test_<suite>.cpp` y `checklist_<suite>.txt`, y añade `<suite>` a `DMG_TEST_SUITES` en `testing/CMakeLists.txt`.

## Limitaciones conocidas y próximos pasos

- Leer `0xFEA0–0xFEFF` se sale del array de OAM (debería devolver un valor fijo).
- Saltos y llamadas: `JP`, `JR`, `CALL`, `RET`, `RETI`, `RST` (con condiciones `NZ Z NC C`).
- Resto del prefijo `CB`: `BIT`, `RES`, `SET`.
- Validar los M-cycles también en las suites anteriores a las misceláneas.
- MBC y RAM externa para cartuchos de más de 32 KB.
- PPU, timers, joypad (salida de `STOP`) y audio.
- Validación con ROMs de prueba (blargg `cpu_instrs`) cuando existan saltos y llamadas.

## Documentación técnica

- [`doc/components/cpu_analisis.txt`](doc/components/cpu_analisis.txt): registros, flags, ciclo de CPU, opcodes y timing.
- [`doc/components/memory_analisis.txt`](doc/components/memory_analisis.txt): mapa de memoria y diseño previsto para MBC.
- [`doc/opcodes_mapping/checklist_test_GB_emulator_26092026.txt`](doc/opcodes_mapping/checklist_test_GB_emulator_26092026.txt): cobertura de opcodes, suites y hallazgos.
- [`doc/ideas/conceptos.txt`](doc/ideas/conceptos.txt): ROM, RAM, pila, `PC`, `SP`, M-Cycles y T-Cycles.
- [`doc/ideas/diseños.txt`](doc/ideas/dise%C3%B1os.txt): templates y operaciones que usan `[HL]`.
- [`doc/ideas/notas.txt`](doc/ideas/notas.txt): rotaciones y desplazamientos.

## Aviso

Este proyecto tiene fines educativos y de investigación. No incluye ROMs comerciales ni archivos de BIOS.
