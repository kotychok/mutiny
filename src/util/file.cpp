#include "file.h"

#include <fstream>
#include <iostream>
#include <sstream>

std::string File::read(const std::string path) {
  std::string fileContents;
  std::ifstream fileStream;

  // Ensure ifstream objects can throw exceptions
  fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    fileStream.open(path);

    std::stringstream stringStream;

    stringStream << fileStream.rdbuf();

    fileStream.close();

    fileContents = stringStream.str();
  } catch (std::ifstream::failure e) {
    std::cerr << "ERROR: File not successfully read. Path: " << path << std::endl;
  }

  return fileContents;
}
