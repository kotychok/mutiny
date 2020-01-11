#pragma once

#include <vector>

#include "chunk.h"
#include "quad.h"

class MesherGreedy {
  public:
    static std::vector<quad> chunkToQuads(const Chunk &chunk);
    static quad_mesh quadToQuadMesh(const quad &quad);
};
