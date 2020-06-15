#pragma once

#include <string>

#include <mruby.h>

typedef std::int_fast16_t BlockType;

struct WorldBlock {
  BlockType type;

  bool operator==(const WorldBlock& other) const {
    return this->type == other.type;
  }

  bool operator!=(const WorldBlock& other) const {
    return !(*this == other);
  }
};

class Block {
  public:
    static const BlockType EMPTY_BLOCK_TYPE = 0;
    static constexpr struct WorldBlock EMPTY_BLOCK { EMPTY_BLOCK_TYPE };
};
