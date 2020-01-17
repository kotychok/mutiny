#pragma once

#include <array>

#include "block.h"
#include "constants.h"

class ChunkGenerator {
  public:
    static std::array<Block, CHUNK_SIZE_CUBED> flat();
    static std::array<Block, CHUNK_SIZE_CUBED> halfSphere();
};
