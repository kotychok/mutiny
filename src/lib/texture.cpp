#include <iostream>

#include <unordered_map>

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "texture.h"

Texture::Texture(std::string imagePath) {
  glGenTextures(1, &ID);
  glBindTexture(GL_TEXTURE_2D, ID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  int width, height, nrChannels;
  unsigned char *data = stbi_load(imagePath.c_str(), &width, &height, &nrChannels, 0);
  if (data) {
    GLint internalFormat;
    GLenum format;

    switch (nrChannels) {
      case 3:
        internalFormat = GL_RGB8;
        format = GL_RGB;
        break;
      case 4:
        internalFormat = GL_RGBA8;
        format = GL_RGBA;
        break;
      default:
        std::cerr << "Error: Unsupported number of channels in texture: " << imagePath << std::endl;
        break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // glTexStorage2D segfaults for some reason.
    // glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, width, height);
    // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
  } else {
    std::cerr << "ERROR: Failed to load image. Path: " << imagePath << std::endl;
  }
  stbi_image_free(data);
}

void Texture::use() {
  glBindTexture(GL_TEXTURE_2D, ID);
  // Pretty sure I want to put TexParam calls here b/c those also care about
  // active texture. Which means they need to be set each time in case the
  // active texture image unit has changed.
}

Texture& Texture::fetch(BlockType blockType) {
  static std::unordered_map<BlockType, Texture> loadedTextures {};

  if (loadedTextures.find(blockType) == loadedTextures.end()) {
    Texture &texture = loadedTextures.try_emplace(blockType, imagePath(blockType)).first->second;
    return texture;
  } else {
    Texture &texture = loadedTextures.find(blockType)->second;
    return texture;
  }
}

std::string Texture::imagePath(BlockType blockType) {
  std::string imagePath {};
  switch (blockType) {
    case BlockType::BEDROCK:
      imagePath = "bedrock.png";
      break;
    case BlockType::DIRT:
      imagePath = "dirt.png";
      break;
    case BlockType::STONE:
      imagePath = "stone.png";
      break;
    case BlockType::COBBLESTONE:
      imagePath = "cobblestone.png";
      break;
    default:
      imagePath = "missing.png";
      break;
  }
  return "./assets/" + imagePath;
}
