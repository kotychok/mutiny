#pragma once

#include <vector>

#include "chunk.h"
#include "quad.h"

class MesherGreedy {
  public:
    static std::vector<float> computeChunkMesh(const Chunk &chunk);

  private:
    static void loadQuadIntoMesh(const quad& quad, const BlockType& blockType, std::vector<float>& mesh);
};
