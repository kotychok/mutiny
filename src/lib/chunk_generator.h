#pragma once

#include <array>

#include "block.h"
#include "constants.h"

class ChunkGenerator {
  public:
    static std::array<Block, CHUNK_SIZE_CUBED> flat();
    static std::array<Block, CHUNK_SIZE_CUBED> flatHalfAndHalf();
    static std::array<Block, CHUNK_SIZE_CUBED> filled();
    static std::array<Block, CHUNK_SIZE_CUBED> half();
    static std::array<Block, CHUNK_SIZE_CUBED> halfSphere();
};
