#include "texture_unit.h"

std::vector<GLenum> TextureUnit::textureUnits {};

int TextureUnit::reserveTextureUnit() {
  static unsigned int currentIndex { 0 };
  textureUnits.push_back(GL_TEXTURE0 + currentIndex);
  return static_cast<int>(currentIndex++);
}

void TextureUnit::activate(int index) {
  glActiveTexture(textureUnits.at(static_cast<unsigned int>(index)));
}
