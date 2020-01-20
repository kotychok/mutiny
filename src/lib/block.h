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

  bool operator==(const Block& other) const {
    return this->type == other.type;
  }

  bool operator!=(const Block& other) const {
    return !(*this == other);
  }
};

static const struct Block EmptyBlock { BlockType::EMPTY };
