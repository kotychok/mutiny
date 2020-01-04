#pragma once

#include "shader.h"

class Chunk {
  public:
    // Constants
    static const int SIZE;

    // Instance variables
    glm::vec3 pos; // Center of the chunk

    // Constructor
    Chunk(glm::vec3 pos);

    // Instance methods
    void render(const Shader &myShader, const glm::mat4 &chunkModel);
};
