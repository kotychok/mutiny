#include <cmath>

#include <noise/noise.h>

#include "chunk_generator.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
std::array<WorldBlock, CHUNK_SIZE_CUBED> ChunkGenerator::flat(glm::vec3 position) {
  std::array<WorldBlock, CHUNK_SIZE_CUBED> blocks {};
  unsigned int y = 0;
  for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
    for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
      unsigned int index { z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x };
      blocks[index] = Block::createWorldBlock("dirt");
    }
  }
  return blocks;
}

std::array<WorldBlock, CHUNK_SIZE_CUBED> ChunkGenerator::flatHalfAndHalf(glm::vec3 position) {
  std::array<WorldBlock, CHUNK_SIZE_CUBED> blocks {};
  unsigned int y = 0;
  for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
    for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
      unsigned int index { z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x };
      if (x < CHUNK_SIZE_HALVED) {
        blocks[index] = Block::createWorldBlock("dirt");
      } else {
        blocks[index] = Block::createWorldBlock("stone");
      }
    }
  }
  return blocks;
}

std::array<WorldBlock, CHUNK_SIZE_CUBED> ChunkGenerator::flatHalfAndHalfWithSquare(glm::vec3 position) {
  std::array<WorldBlock, CHUNK_SIZE_CUBED> blocks {};
  unsigned int y = 0;
  for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
    for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
      unsigned int index { z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x };
      if (x > CHUNK_SIZE / 4 && x < 3 * CHUNK_SIZE / 4 && z > CHUNK_SIZE / 4 && z < 3 * CHUNK_SIZE / 4) {
        blocks[index] = Block::createWorldBlock("bedrock");
      } else if (x < CHUNK_SIZE_HALVED) {
        blocks[index] = Block::createWorldBlock("dirt");
      } else {
        blocks[index] = Block::createWorldBlock("stone");
      }
    }
  }
  return blocks;
}

std::array<WorldBlock, CHUNK_SIZE_CUBED> ChunkGenerator::flatRandom(glm::vec3 position) {
  std::array<WorldBlock, CHUNK_SIZE_CUBED> blocks {};
  unsigned int y = 0;
  for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
    for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
      unsigned int index { z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x };
      if (rand() % 100 < 50) {
        blocks[index] = Block::createWorldBlock("dirt");
      } else {
        blocks[index] = Block::createWorldBlock("stone");
      }
    }
  }
  return blocks;
}

std::array<WorldBlock, CHUNK_SIZE_CUBED> ChunkGenerator::flatWithPlus(glm::vec3 position) {
  std::array<WorldBlock, CHUNK_SIZE_CUBED> blocks {};
  if (position.y != 0) {
    return blocks;
  }

  for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
    for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
      for (unsigned int y = 0; y < CHUNK_SIZE / 4; y++) {
        unsigned int index { z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x };
        if (y == 0) {
          blocks[index] = Block::createWorldBlock("bedrock");
        } else if (x == CHUNK_SIZE_HALVED || z == CHUNK_SIZE_HALVED) {
          blocks[index] = Block::createWorldBlock("dirt");
        }
      }
    }
  }
  return blocks;
}


std::array<WorldBlock, CHUNK_SIZE_CUBED> ChunkGenerator::half(glm::vec3 position) {
  std::array<WorldBlock, CHUNK_SIZE_CUBED> blocks {};
  for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
    for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
      for (unsigned int y = 0; y < CHUNK_SIZE_HALVED; y++) {
        unsigned int index { z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x };
        blocks[index] = Block::createWorldBlock("dirt");
      }
    }
  }
  return blocks;
}

std::array<WorldBlock, CHUNK_SIZE_CUBED> ChunkGenerator::filled(glm::vec3 position) {
  std::array<WorldBlock, CHUNK_SIZE_CUBED> blocks {};
  for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
    for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
      for (unsigned int y = 0; y < CHUNK_SIZE; y++) {
        unsigned int index { z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x };
        blocks[index] = Block::createWorldBlock("dirt");
      }
    }
  }
  return blocks;
}

std::array<WorldBlock, CHUNK_SIZE_CUBED> ChunkGenerator::halfSphere(glm::vec3 position) {
  std::array<WorldBlock, CHUNK_SIZE_CUBED> blocks {};
  int radius = CHUNK_SIZE_HALVED;
  for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
    for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
      for (unsigned int y = 0; y < CHUNK_SIZE; y++) {
        if (y < 8) {
          unsigned int blockX = x - CHUNK_SIZE / 2;
          unsigned int blockY = y - CHUNK_SIZE / 2;
          unsigned int blockZ = z - CHUNK_SIZE / 2;
          if (sqrt(blockX * blockX + blockY * blockY + blockZ * blockZ) < radius) {
            unsigned int index { z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x };
            blocks[index] = Block::createWorldBlock("dirt");
          }
        }
      }
    }
  }
  return blocks;
}

std::array<WorldBlock, CHUNK_SIZE_CUBED> ChunkGenerator::perlin(glm::vec3 position) {
  std::array<WorldBlock, CHUNK_SIZE_CUBED> blocks {};
  if (position.y <= 1) {
    for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
      for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
        for (unsigned int y = 0; y < CHUNK_SIZE; y++) {
          unsigned int index { z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x };
          blocks[index] = Block::createWorldBlock("stone");
        }
      }
    }
  } else if (position.y == 2) {
    noise::module::Perlin perlinNoise;
    const int min { 0 };
    const int max { CHUNK_SIZE };

    for (unsigned int blockX = 0; blockX < CHUNK_SIZE; ++blockX) {
      for (unsigned int blockZ = 0; blockZ < CHUNK_SIZE; ++blockZ) {
        double noiseX = static_cast<float>(blockX) / CHUNK_SIZE + position.x;
        double noiseZ = static_cast<float>(blockZ) / CHUNK_SIZE + position.z;

        double noise = std::clamp((1 + perlinNoise.GetValue(noiseX, 0.0, noiseZ)) / 2.0, 0.0, 1.0);
        unsigned int height = static_cast<unsigned int>(std::clamp(static_cast<int>(min + noise * (max + 1 - min)), 0, CHUNK_SIZE));
        for (unsigned int y = 0; y < height; y++) {
          unsigned int index { blockZ * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + blockX };
          if (y <= height - 3) {
            blocks[index] = Block::createWorldBlock("stone");
          } else {
            blocks[index] = Block::createWorldBlock("grass");
          }
        }
      }
    }
  }
  return blocks;
}
#pragma GCC diagnostic pop
