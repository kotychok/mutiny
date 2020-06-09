#pragma once

#include <cstdint>
#include <unordered_map>

#include <mini-yaml/yaml.hpp>

// BlockType is simply the index in blocks.yml for a particular block.
//
// This yaml data will be availble from Blocks::blocksData().
//
// When world persistence is done, we will save the mapping of block id to
// block type in order to not ruin worlds if the order in the file changes. On
// load, if a new block id is preset that doesn't have an integer block type
// assigned, it will get one from the remaining pool of unused ones.
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

    static Yaml::Node blocksDataCache;
    static bool blocksDataCacheLoaded;
    static Yaml::Node& blocksData();

    static std::unordered_map<std::string, BlockType> blockIdToBlockTypeCache;
    static bool blockIdToBlockTypeCacheLoaded;
    static std::unordered_map<std::string, BlockType>& blockIdToBlockType();

    static WorldBlock createWorldBlock(std::string blockId);
};

