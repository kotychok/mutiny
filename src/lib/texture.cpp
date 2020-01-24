#include <iostream>

#include <unordered_map>

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "texture.h"

const std::unordered_map<BlockType, std::string> Texture::blockTypeToString {
  {
    { BlockType::BEDROCK, "bedrock" },
    { BlockType::COBBLESTONE, "cobblestone" },
    { BlockType::DIRT, "dirt" },
    { BlockType::STONE, "stone" },
  }
};

const int Texture::BLOCK_COUNT = blockTypeToString.size();

const std::unordered_map<BlockType, float> Texture::blockTypeToTextureIndex {
  {
    { BlockType::BEDROCK, 0 },
    { BlockType::COBBLESTONE, 1 },
    { BlockType::DIRT, 2 },
    { BlockType::STONE, 3 },
  }
};

void Texture::loadBlockTextures() {
  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);

  glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 16, 16, BLOCK_COUNT);

  for(auto it : blockTypeToString) {
    const BlockType& blockType = it.first;
    const std::string& blockName = it.second;
    float textureIndex { blockTypeToTextureIndex.find(blockType)->second };

    std::string path = "./assets/" + blockName + ".png";

    int width, height, nrChannels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    glTexSubImage3D(
      GL_TEXTURE_2D_ARRAY,
      0,                 // mipmap level
      0,                 // xoffset
      0,                 // yoffset
      textureIndex,      // zoffset, i.e. index in texture array
      width,             // width
      height,            // height
      1,                 // depth
      GL_RGBA,           // cpu pixel format
      GL_UNSIGNED_BYTE,  // cpu pixel coord type
      data               // pixel data
    );

    stbi_image_free(data);
  }

  glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}
