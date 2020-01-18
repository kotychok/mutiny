#pragma once

#include <array>
#include <functional>
#include <vector>

#include "block.h"
#include "chunk_generator.h"
#include "constants.h"
#include "quad.h"
#include "shader.h"

class Chunk {
  public:
    // Instance variables
    glm::vec3 pos;
    std::function<std::array<Block, CHUNK_SIZE_CUBED>()> chunkGeneratorFunc;

    // Constructor
    Chunk(glm::vec3 pos, std::function<std::array<Block, CHUNK_SIZE_CUBED>()> chunkGeneratorFunc);
    ~Chunk();

    // Explicitly delete since these shouldn't be used
    Chunk(const Chunk&) = delete; // Delete copy constructor
    Chunk& operator=(const Chunk&) = delete; // Delete copy assignment
    Chunk(Chunk&&) = delete; // Delete move constructor
    Chunk& operator=(Chunk &&) = delete; // Delete move assignment

    // Instance methods
    void setMesh(std::vector<std::pair<quad_mesh, BlockType>> quadMeshes);
    void render(const Shader &myShader);
    bool isBlockAt(unsigned int x, unsigned int y, unsigned int z) const;

  private:
    unsigned int chunkVAO {};
    unsigned int chunkVBO {};

    std::array<Block, CHUNK_SIZE_CUBED> blocks {};

    std::vector<std::pair<quad_mesh, BlockType>> quadMeshes {};
};
