#pragma once

#include <array>
#include <vector>

#include <glm/gtx/scalar_multiplication.hpp>
#include <mruby.h>

#include "block.h"
#include "constants.h"
#include "shader.h"

class Chunk {
  public:
    // Instance variables
    glm::vec3 pos;
    std::string chunkGeneratorFunc;

    // Constructor
    Chunk(glm::vec3 pos, std::string chunkGeneratorFunc);
    ~Chunk();

    // Explicitly delete since these shouldn't be used
    Chunk(const Chunk&) = delete; // Delete copy constructor
    Chunk& operator=(const Chunk&) = delete; // Delete copy assignment
    Chunk(Chunk&&) = delete; // Delete move constructor
    Chunk& operator=(Chunk &&) = delete; // Delete move assignment

    // Instance methods
    void generate();
    void setMesh(std::vector<float> mesh);
    void render(const Shader &shader);
    bool isBlockAt(unsigned int x, unsigned int y, unsigned int z) const;
    WorldBlock blockAt(unsigned int x, unsigned int y, unsigned int z) const;

  private:
    unsigned int chunkVAO {};
    unsigned int chunkVBO {};

    std::array<WorldBlock, CHUNK_SIZE_CUBED> blocks {};

    std::vector<float> mesh {};
    bool isMeshDirty { false };
};
