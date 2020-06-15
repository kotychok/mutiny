#pragma once

#include <vector>

#include <glad/glad.h>

class TextureUnit {
  public:
    static std::vector<GLenum> textureUnits;

    static int reserveTextureUnit();
    static void activate(int textureUnitIndex);
};
