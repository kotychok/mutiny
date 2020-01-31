#pragma once

#include <array>
#include <functional>

#include <glm/glm.hpp>

#include "block.h"
#include "constants.h"

class ChunkGenerator {
  public:
    static std::array<Block, CHUNK_SIZE_CUBED> flat(glm::vec3 position);
    static std::array<Block, CHUNK_SIZE_CUBED> flatHalfAndHalf(glm::vec3 position);
    static std::array<Block, CHUNK_SIZE_CUBED> flatHalfAndHalfWithSquare(glm::vec3 position);
    static std::array<Block, CHUNK_SIZE_CUBED> flatRandom(glm::vec3 position);
    static std::array<Block, CHUNK_SIZE_CUBED> flatWithPlus(glm::vec3 position);
    static std::array<Block, CHUNK_SIZE_CUBED> filled(glm::vec3 position);
    static std::array<Block, CHUNK_SIZE_CUBED> half(glm::vec3 position);
    static std::array<Block, CHUNK_SIZE_CUBED> halfSphere(glm::vec3 position);
    static std::array<Block, CHUNK_SIZE_CUBED> perlin(glm::vec3 position);
};
