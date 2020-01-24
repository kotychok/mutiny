#pragma once

#include <vector>

#include "chunk.h"
#include "quad.h"

class MesherGreedy {
  public:
    static std::vector<quad_mesh> chunkToQuads(const Chunk &chunk);

  private:
    static quad_mesh quadToQuadMesh(const quad& quad, const BlockType& blockType);
};
