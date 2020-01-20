#pragma once

#include <stdint.h>

enum class BlockType {
  EMPTY = 0,
  BEDROCK,
  DIRT,
  STONE,
  COBBLESTONE,
};

struct Block {
  BlockType type;
};

static const struct Block EmptyBlock { BlockType::EMPTY };
