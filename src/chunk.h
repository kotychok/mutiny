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

    // Instance methods
    void render(const Shader &myShader, const glm::mat4 &chunkModel);
};
