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
    glm::vec3 pos;

    // Constructor
    Chunk(glm::vec3 pos);
    ~Chunk();

    // Explicitly delete since these shouldn't be used
    Chunk(const Chunk&) = delete; // Delete copy constructor
    Chunk& operator=(const Chunk&) = delete; // Delete copy assignment
    Chunk(Chunk&&) = delete; // Delete move constructor
    Chunk& operator=(Chunk &&) = delete; // Delete move assignment

    // Instance methods
    void setMesh(std::vector<quad_mesh> quadMeshes);
    void render(const Shader &myShader);
    bool isBlockAt(int x, int y, int z) const;

  private:
    unsigned int chunkVAO {};
    unsigned int chunkVBO {};

    Block blocks[SIZE * SIZE * SIZE] {};

    std::vector<quad_mesh> quadMeshes {};
};
