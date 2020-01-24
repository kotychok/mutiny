#pragma once

#include <string>
#include <unordered_map>

#include "block.h"

class Texture {
  public:
    static const std::unordered_map<BlockType, std::string> blockTypeToString;
    static std::unordered_map<BlockType, float> blockTypeToTextureIndex;
    static const int BLOCK_COUNT;

    static void loadBlockTextures();
};
