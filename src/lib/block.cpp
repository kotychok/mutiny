#include "block.h"

Yaml::Node Block::blocksDataCache {};

bool Block::blocksDataCacheLoaded { false };

Yaml::Node& Block::blocksData() {
  if (!blocksDataCacheLoaded) {
    Yaml::Parse(blocksDataCache, "./assets/blocks.yml");
    blocksDataCacheLoaded = true;
  }
  return blocksDataCache;
}


std::unordered_map<std::string, BlockType> Block::blockIdToBlockTypeCache {};

bool Block::blockIdToBlockTypeCacheLoaded { false };

std::unordered_map<std::string, BlockType>& Block::blockIdToBlockType() {
  if (!blockIdToBlockTypeCacheLoaded) {
    Yaml::Node& blocksData = Block::blocksData();
    int i = 1; // Start at 1 since 0 is the empty block
    for (auto it = blocksData.Begin(); it != blocksData.End(); it++) {
      Yaml::Node& blockNode = (*it).second;
      std::string id = blockNode["id"].As<std::string>();
      blockIdToBlockTypeCache[id] = i;
      i++;
    }
    blockIdToBlockTypeCacheLoaded = true;
  }
  return blockIdToBlockTypeCache;
}


WorldBlock Block::createWorldBlock(std::string blockId) {
  std::unordered_map<std::string, BlockType>& mapping = Block::blockIdToBlockType();
  return WorldBlock { mapping[blockId] };
}
