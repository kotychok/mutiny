#pragma once

#include <string>
#include <unordered_map>

#include "block.h"
#include "side.h"
#include "ruby_vm.h"

class Texture {
  public:
    static void loadBlockTextures();
    static void loadAstronomicalBodiesTextures();
    static float getTextureIndexFromBlockType(BlockType blockType, Side side);

  private:
    static mrb_state *s_mrb;
    static mrb_value s_mrbLoadsBlockTextureAtlasResult;
    static mrb_value s_mrbBlockTextureAtlas;

    static void loadBlockImageIntoTexture(std::string path, float textureIndex);
};
