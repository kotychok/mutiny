#pragma once

#include "shader.h"

class Chunk {
  public:
    // Constants
    static constexpr int SIZE { 16 };

    // Instance variables
    glm::vec3 pos; // Center of the chunk
    int blocks[SIZE * SIZE * SIZE] {};

    // Constructor
    Chunk(glm::vec3 pos);
    ~Chunk();

    // Explicitly delete since these shouldn't be used
    Chunk(const Chunk&) = delete; // Delete copy constructor
    Chunk& operator=(const Chunk&) = delete; // Delete copy assignment
    Chunk(Chunk&&) = delete; // Delete move constructor
    Chunk& operator=(Chunk &&) = delete; // Delete move assignment

    // Instance methods
    void render(const Shader &myShader, const glm::mat4 &chunkModel);
};
