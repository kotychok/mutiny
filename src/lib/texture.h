#pragma once

#include <string>

#include "block.h"

class Texture {
  public:
    static Texture& fetch(BlockType blockType);
    static std::string imagePath(BlockType blockType);

    unsigned int ID {};

    Texture(std::string imagePath);

    // Explicitly delete since these shouldn't be used
    Texture(const Texture&) = delete; // Delete copy constructor
    Texture& operator=(const Texture&) = delete; // Delete copy assignment
    Texture(Texture&&) = delete; // Delete move constructor
    Texture& operator=(Texture &&) = delete; // Delete move assignment

    void use();
};
