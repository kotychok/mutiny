#pragma once

#include <string>
#include <unordered_map>

#include "block.h"
#include "side.h"

class Texture {
  public:
    static float getTextureIndexFromBlockType(BlockType blockType, Side side);
    static void loadBlockTextures();
    static void loadAstronomicalBodiesTextures();

  private:
    static std::unordered_map<BlockType, std::unordered_map<Side, float>> blockTypeToSideToTextureIndex;
    static void loadBlockImageIntoTexture(std::string path, float textureIndex);
};
