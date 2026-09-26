# Game Boy C++ Emulator

Emulador experimental de la **Nintendo Game Boy clásica (DMG)**, escrito en C++ y orientado al aprendizaje de la arquitectura del hardware: ciclo *fetch-decode-execute*, registros de la CPU, tabla de opcodes, banderas y mapa de memoria.

El estado actual del proyecto se centra en construir el núcleo de la CPU y el bus de memoria. El ejecutable disponible funciona como un banco de pruebas que carga una ROM sintética en memoria y muestra el estado de los registros después de cada ciclo de CPU.

> **Estado del proyecto:** prototipo en desarrollo. Todavía no es un emulador completo capaz de ejecutar juegos comerciales.

## Características actuales

- CPU de 8 bits con registros `A`, `F`, `B`, `C`, `D`, `E`, `H` y `L`.
- Registros especiales `PC` (*Program Counter*) y `SP` (*Stack Pointer*).
- Combinación de registros de 8 bits en pares de 16 bits: `AF`, `BC`, `DE` y `HL`.
- Tabla de 256 entradas basada en punteros a métodos miembro de `CPU`.
- Protección para opcodes no implementados mediante `b_illegal_opcode()`.
- Flujo de ejecución `fetch → decode → execute` en `CPU::cpuCycle()`.
- Implementación parcial de instrucciones de la familia:
  - `NOP` (`0x00`).
  - `LD r8, imm8`.
  - `LD r8, r8`.
  - Lecturas y escrituras indirectas con `BC`, `DE`, `HL+` y `HL-`.
  - `INC` sobre registros de 8 bits, memoria apuntada por `HL`, registros de 16 bits y `SP`.
  - Implementación del código fuente de `DEC`, pendiente de integrarse en la tabla activa de opcodes y en la compilación principal.
- Banderas `Z`, `N`, `H` y `C` almacenadas en el registro `F`; los cuatro bits inferiores se mantienen en cero.
- Bus de memoria con regiones para ROM, VRAM, WRAM, OAM, registros de E/S, HRAM y registro `IE`.
- Utilidad para leer archivos binarios como vectores de bytes, preparada para cargar ROMs reales.
- Generación de `compile_commands.json` mediante CMake.

## Arquitectura

El proyecto separa el procesador, el bus de memoria y la lógica de instrucciones:

```text
main.cpp
   │
   ├── crea BUS y CPU
   ├── carga una ROM de prueba en BUS
   └── ejecuta CPU::cpuCycle() y muestra el estado

CPU
   ├── obtiene el opcode desde BUS::read(PC)
   ├── incrementa PC
   ├── busca la función en opcode_table[256]
   └── ejecuta la instrucción seleccionada

BUS
   ├── traduce direcciones de 16 bits a regiones de memoria
   ├── devuelve bytes de la ROM o de las memorias internas
   └── será responsable de soportar escrituras y futuros MBC
```

### CPU y decodificación

`CPU` inicializa `PC` en `0x0100`, que es la dirección de inicio habitual del programa después de la BIOS de la Game Boy. Cada entrada de `opcode_table` contiene un puntero a una función con la forma `int (CPU::*)()`. El valor devuelto representa los **M-Cycles** consumidos por la instrucción; en el modelo documentado, un M-Cycle equivale a cuatro T-Cycles.

Los cargadores de opcodes en `utils/OpcodesLoaders.cpp` rellenan la tabla por familias. Las operaciones genéricas se implementan con templates en `cpuinstructions/`, reduciendo la duplicación entre registros y modos de direccionamiento.

### Bus de memoria

`BUS::read()` utiliza un mapa de direcciones de 16 bits con las siguientes regiones principales:

| Rango | Región | Estado actual |
|---|---|---|
| `0x0000–0x7FFF` | ROM del cartucho / BIOS mapeada | Lectura implementada |
| `0x8000–0x9FFF` | VRAM | Lectura implementada |
| `0xA000–0xBFFF` | RAM externa | Devuelve `0x00`; MBC pendiente |
| `0xC000–0xDFFF` | WRAM | Lectura implementada |
| `0xE000–0xFDFF` | Echo RAM | Mapeada sobre WRAM |
| `0xFE00–0xFEFF` | OAM | Lectura implementada |
| `0xFF00–0xFF7F` | Registros de E/S | Lectura implementada |
| `0xFF80–0xFFFE` | HRAM | Lectura implementada |
| `0xFFFF` | Interrupt Enable | Lectura implementada |

La operación `BUS::write()` todavía es un stub de diagnóstico: imprime la dirección y el valor, pero aún no modifica las regiones de memoria. Por tanto, las instrucciones que escriben en memoria no representan todavía el comportamiento final del hardware.

## Estructura del repositorio

```text
.
├── CMakeLists.txt              # Configuración del ejecutable DMGE
├── main.cpp                    # ROM sintética y bucle de demostración
├── include/                    # Interfaces CPU, BUS, loaders y utilidades
├── src/                        # Implementaciones de CPU y bus de memoria
├── cpuinstructions/            # Familias de instrucciones LD, INC y DEC
├── utils/                      # Registro de opcodes y lectura de archivos
├── doc/
│   ├── components/             # Análisis del CPU y del mapa de memoria
│   ├── ideas/                  # Notas de diseño y conceptos de arquitectura
│   └── lazygit.txt             # Atajos de trabajo con LazyGit
└── build/                      # Directorio local de artefactos de compilación
```

Los archivos de `build/` son artefactos locales y no forman parte de la configuración necesaria para compilar desde cero. El `.gitignore` excluye los archivos habituales generados por CMake y CLion.

## Requisitos

- Compilador compatible con C++17 o posterior.
- CMake `3.16` o posterior.
- Git.

No se utilizan dependencias externas ni librerías de terceros en el `CMakeLists.txt` actual.

## Compilar y ejecutar

Desde la raíz del repositorio:

```bash
cmake -S . -B build
cmake --build build
./build/DMGE
```

En Windows, el ejecutable puede quedar dentro de una subcarpeta generada por el backend de CMake, por ejemplo `build/Debug/DMGE.exe`.

El programa actual no recibe todavía una ruta de ROM por argumentos. `main.cpp` construye una ROM de prueba de 256 bytes de relleno seguida de instrucciones que ejercitan `LD` e `INC`, crea un `BUS`, crea una `CPU`, carga la ROM y muestra información como `PC`, `SP`, registros, banderas y opcode ejecutado.

## Añadir o modificar instrucciones

1. Declara la operación en `include/CPU.h`.
2. Implementa la lógica en el archivo de la familia correspondiente dentro de `cpuinstructions/`.
3. Añade las instanciaciones explícitas de los templates necesarios.
4. Registra los opcodes en `utils/OpcodesLoaders.cpp`.
5. Si la familia debe compilarse, incorpora su `.cpp` a `CMakeLists.txt`.
6. Añade una secuencia pequeña a `testInstructions` en `main.cpp` y comprueba los registros, las banderas y el `PC`.

Los opcodes que no se hayan registrado quedan apuntando a `b_illegal_opcode()`, que informa del opcode y del `PC` antes de detener el proceso.

## Limitaciones conocidas y próximos pasos

- Implementar completamente `BUS::write()` y revisar las regiones de E/S.
- Integrar correctamente la carga y el mapeo de la boot ROM.
- Conectar `read_binary_file()` con el flujo de carga de una ROM proporcionada por el usuario.
- Activar y validar la familia `DEC` en el cargador y en el `CMakeLists.txt`.
- Completar el conjunto de instrucciones del LR35902, incluyendo `HALT`, `STOP`, saltos, pila, llamadas, retornos, operaciones lógicas y operaciones con prefijo `CB`.
- Añadir pruebas automatizadas para registros, flags, direccionamiento y tiempos.
- Implementar MBC y RAM externa para soportar cartuchos de mayor tamaño.
- Incorporar PPU, temporizador, interrupciones, entrada y salida de audio para avanzar hacia una emulación completa.
- Sustituir la ROM sintética de `main.cpp` por una interfaz de carga de juegos y un bucle de emulación controlado.

## Documentación técnica

- [`doc/components/cpu_analisis.txt`](doc/components/cpu_analisis.txt): registros, flags, ciclo de CPU, opcodes y timing.
- [`doc/components/memory_analisis.txt`](doc/components/memory_analisis.txt): mapa de memoria y diseño previsto para MBC.
- [`doc/ideas/conceptos.txt`](doc/ideas/conceptos.txt): conceptos de ROM, RAM, pila, `PC`, `SP`, M-Cycles y T-Cycles.
- [`doc/ideas/diseños.txt`](doc/ideas/dise%C3%B1os.txt): notas sobre templates y operaciones que utilizan `[HL]`.

## Aviso

Este proyecto tiene fines educativos y de investigación. No incluye ROMs comerciales ni archivos de BIOS. Utiliza únicamente datos de prueba definidos en el código fuente.
