#include <cmath>

#include "chunk_generator.h"

std::array<Block, CHUNK_SIZE_CUBED> ChunkGenerator::flat() {
  std::array<Block, CHUNK_SIZE_CUBED> blocks {};
  unsigned int y = 0;
  for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
    for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
      unsigned int index { z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x };
      blocks[index] = Block { BlockType::DIRT };
    }
  }
  return blocks;
}

std::array<Block, CHUNK_SIZE_CUBED> ChunkGenerator::flatHalfAndHalf() {
  std::array<Block, CHUNK_SIZE_CUBED> blocks {};
  unsigned int y = 0;
  for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
    for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
      unsigned int index { z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x };
      if (x < CHUNK_SIZE_HALVED) {
        blocks[index] = Block { BlockType::DIRT };
      } else {
        blocks[index] = Block { BlockType::STONE };
      }
    }
  }
  return blocks;
}

std::array<Block, CHUNK_SIZE_CUBED> ChunkGenerator::half() {
  std::array<Block, CHUNK_SIZE_CUBED> blocks {};
  for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
    for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
      for (unsigned int y = 0; y < CHUNK_SIZE_HALVED; y++) {
        unsigned int index { z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x };
        blocks[index] = Block { BlockType::DIRT };
      }
    }
  }
  return blocks;
}

std::array<Block, CHUNK_SIZE_CUBED> ChunkGenerator::filled() {
  std::array<Block, CHUNK_SIZE_CUBED> blocks {};
  for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
    for (unsigned int z = 0; z < CHUNK_SIZE; z++) {
      for (unsigned int y = 0; y < CHUNK_SIZE; y++) {
        unsigned int index { z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x };
        blocks[index] = Block { BlockType::DIRT };
      }
    }
  }
  return blocks;
}

std::array<Block, CHUNK_SIZE_CUBED> ChunkGenerator::halfSphere() {
  std::array<Block, CHUNK_SIZE_CUBED> blocks {};
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
            blocks[index] = Block { BlockType::DIRT };
          }
        }
      }
    }
  }
  return blocks;
}
