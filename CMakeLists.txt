set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required(VERSION 3.16)
project(DMGEmulator CXX)

add_executable(DMGE 
  main.cpp
  src/CPU.cpp
  src/MemoryBus.cpp
  src/CPU_LD_Opcodes.cpp
  utils/OpcodesLoaders.cpp
  utils/FileUtils.cpp
)
