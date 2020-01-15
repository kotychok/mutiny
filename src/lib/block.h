#pragma once

#include <stdint.h>

enum class BlockType {
  AIR = 0,
  BEDROCK,
  DIRT,
  STONE,
};

struct Block {
  BlockType type;
};
