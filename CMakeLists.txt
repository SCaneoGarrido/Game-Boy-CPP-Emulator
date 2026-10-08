set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required(VERSION 3.16)
project(DMGEmulator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Nucleo del emulador (CPU + BUS + opcodes). Lo comparten DMGE y los tests.
# GLOB con CONFIGURE_DEPENDS: un archivo nuevo en src/cpuinstructions/ entra
# solo en el build, sin tener que listarlo aqui.
file(GLOB DMG_CORE_SOURCES CONFIGURE_DEPENDS
  src/*.cpp
  src/cpuinstructions/*.cpp
)
add_library(dmg_core STATIC ${DMG_CORE_SOURCES}
  utils/OpcodesLoaders.cpp
  utils/FileUtils.cpp
)
target_include_directories(dmg_core PUBLIC include)

add_executable(DMGE main.cpp)
target_link_libraries(DMGE PRIVATE dmg_core)

enable_testing()
add_subdirectory(testing)
