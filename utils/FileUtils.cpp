#include "../include/FileUtils.h"
#include <fstream>
#include <ios>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>

std::vector<std::uint8_t> read_binary_file(const std::string& path) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);

  if (!file.is_open()) {
    std::cerr << "Error al abrir el archivo ROM" << std::endl;
    return {};
  }

  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<std::uint8_t> buffer(size);
  if (file.read(reinterpret_cast<char*>(buffer.data()), size)) {
    return buffer;
  }

  return {};
}
