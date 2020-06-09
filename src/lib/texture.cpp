#include <iostream>
#include <unordered_map>

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <mini-yaml/yaml.hpp>

#include "texture.h"

std::unordered_map<BlockType, std::unordered_map<Side, float>> Texture::blockTypeToSideToTextureIndex {};

// Loop over blocks [x]
//   Loop over texture info [x]
//     Normalize the representation to make it easy to find image for any side.
//       Instead of the image path, map the block id and side to the texture layer index
//         Store that and use it in the getTextureIndexFromBlockType
//         e.g.:
//         {
//           Block::BEDROCK => {
//             Side::NORTH => 0,
//             Side::SOUTH => 0,
//             Side::EAST => 0,
//             Side::WEST => 0,
//             Side::TOP => 0,
//             Side::BOTTOM => 0,
//           },
//           Block::GRASS => {
//             Side::NORTH => 1,
//             Side::SOUTH => 1,
//             Side::EAST => 1,
//             Side::WEST => 1,
//             Side::TOP => 2,
//             Side::BOTTOM => 3,
//           }
//         }

void Texture::loadBlockTextures() {
  // Create a new texture array
  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);

  // Configure how many images in the array and the size of each image.
  Yaml::Node& blocksData = Block::blocksData();
  int imageSize = 16;
  int blockCount = blocksData.Size() * 6; // We are being inefficient now and storing duplicates (1 layer for each side per block)
  glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, imageSize, imageSize, blockCount);

  // Loop over the blocks data and save it in a form we can use later for rendering.
  // This involves creating a map similar to the blocks data except instead of
  // using strings to identify the block and the textures, we use the floats.
  float currentBlockType = 1;
  float currentTextureIndex = 0;
  for (auto it = blocksData.Begin(); it != blocksData.End(); it++) {
    Yaml::Node& blockNode = (*it).second;
    Yaml::Node& textureNode = blockNode["texture"];
    std::unordered_map<Side, float> sideToTextureIndex;
    for (auto textureIt = textureNode.Begin(); textureIt != textureNode.End(); textureIt++) {
      std::string sideString = (*textureIt).first;
      Side side;
      if (sideString == "north") side = Side::NORTH;
      else if (sideString == "south") side = Side::SOUTH;
      else if (sideString == "east") side = Side::EAST;
      else if (sideString == "west") side = Side::WEST;
      else if (sideString == "top") side = Side::TOP;
      else if (sideString == "bottom") side = Side::BOTTOM;
      std::string texturePath = (*textureIt).second.As<std::string>();
      // std::cout << "texturePath: " << texturePath << ", textureIndex: " << currentTextureIndex << ", side: " << side << std::endl;
      loadBlockImageIntoTexture(texturePath, currentTextureIndex);
      sideToTextureIndex[side] = currentTextureIndex;
      currentTextureIndex++;
    }

    blockTypeToSideToTextureIndex[currentBlockType] = sideToTextureIndex;
    currentBlockType++;
  }

  glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Texture::loadBlockImageIntoTexture(std::string path, float textureIndex) {
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


float Texture::getTextureIndexFromBlockType(BlockType blockType, Side side) {
  return Texture::blockTypeToSideToTextureIndex.find(blockType)->second.find(side)->second;
}

void Texture::loadAstronomicalBodiesTextures() {
  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);

  int imageSize = 8;
  int numberOfImages = 2;
  glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGB8, imageSize, imageSize, numberOfImages);

  float textureIndex = 0;
  std::string path = "./assets/sun.png";
  int width, height, nrChannels;
  unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
  // std::cout << nrChannels << std::endl;
  glTexSubImage3D(
    GL_TEXTURE_2D_ARRAY,
    0,                 // mipmap level
    0,                 // xoffset
    0,                 // yoffset
    textureIndex,      // zoffset, i.e. index in texture array
    width,             // width
    height,            // height
    1,                 // depth
    GL_RGB,           // cpu pixel format
    GL_UNSIGNED_BYTE,  // cpu pixel coord type
    data               // pixel data
  );
  stbi_image_free(data);

  textureIndex = 1;
  path = "./assets/moon.png";
  data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
  // std::cout << nrChannels << std::endl;
  glTexSubImage3D(
    GL_TEXTURE_2D_ARRAY,
    0,                 // mipmap level
    0,                 // xoffset
    0,                 // yoffset
    textureIndex,      // zoffset, i.e. index in texture array
    width,             // width
    height,            // height
    1,                 // depth
    GL_RGB,           // cpu pixel format
    GL_UNSIGNED_BYTE,  // cpu pixel coord type
    data               // pixel data
  );
  stbi_image_free(data);

  glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}
