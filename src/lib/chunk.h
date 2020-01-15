#pragma once

#include <array>
#include <vector>

#include "block.h"
#include "shader.h"
#include "quad.h"

class Chunk {
  public:
    // Constants
    static constexpr int SIZE { 32 };

    // Instance variables
    glm::vec3 pos; // Center of the chunk
    Block blocks[SIZE * SIZE * SIZE] {};

    // Constructor
    Chunk(glm::vec3 pos);
    ~Chunk();

    // Explicitly delete since these shouldn't be used
    Chunk(const Chunk&) = delete; // Delete copy constructor
    Chunk& operator=(const Chunk&) = delete; // Delete copy assignment
    Chunk(Chunk&&) = delete; // Delete move constructor
    Chunk& operator=(Chunk &&) = delete; // Delete move assignment

    // Instance methods
    void render(const Shader &myShader);
    bool isBlockAt(int x, int y, int z) const;

  private:
    unsigned int chunkVAO {};
    unsigned int chunkVBO {};

    std::vector<quad> quads {};
    std::vector<quad_mesh> quadMeshes {};
};
