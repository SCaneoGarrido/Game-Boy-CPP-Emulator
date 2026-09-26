set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required(VERSION 3.16)
project(DMGEmulator CXX)

add_executable(DMGE 
  main.cpp
  src/CPU.cpp
  src/MemoryBus.cpp
  src/cpuinstructions/CPU_ADD_Opcodes.cpp
  src/cpuinstructions/CPU_AND_Opcodes.cpp
  src/cpuinstructions/CPU_DEC_Opcodes.cpp
  src/cpuinstructions/CPU_INC_Opcodes.cpp
  src/cpuinstructions/CPU_LD_Opcodes.cpp
  src/cpuinstructions/CPU_SUB_Opcodes.cpp
  utils/OpcodesLoaders.cpp
  utils/FileUtils.cpp
)
