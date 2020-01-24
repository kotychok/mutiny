#pragma once

#include <vector>

#include "chunk.h"
#include "quad.h"

class MesherGreedy {
  public:
    static std::vector<std::pair<quad, BlockType>> chunkToQuads(const Chunk &chunk);
    static quad_mesh quadToQuadMesh(const std::pair<quad, BlockType> &pair);
};
