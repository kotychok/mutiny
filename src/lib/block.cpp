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

WorldBlock Block::createWorldBlock(std::string blockId) {
  Yaml::Node& blocksData = Block::blocksData();
  int i = 1; // Start at 1 since 0 is the empty block
  for (auto it = blocksData.Begin(); it != blocksData.End(); it++) {
    Yaml::Node& blockNode = (*it).second;
    std::string id = blockNode["id"].As<std::string>();
    if (blockId == id) {
      return WorldBlock { i };
    }
    i++;
  }
  throw "Could not find block with id " + blockId;
}
