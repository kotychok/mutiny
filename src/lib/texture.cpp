#include <iostream>
#include <unordered_map>

#include <glad/glad.h>
#include <mini-yaml/yaml.hpp>
#include <mruby.h>
#include <mruby/hash.h>
#include <mruby/string.h>
#include <stb/stb_image.h>

#include "texture.h"
#include "side.h"

mrb_state *Texture::s_mrb {};
mrb_value Texture::s_mrbLoadsBlockTextureAtlasResult { mrb_nil_value() };
mrb_value Texture::s_mrbBlockTextureAtlas { mrb_nil_value() };

// std::unordered_map<BlockType, std::unordered_map<Side, float>> Texture::blockTypeToSideToTextureIndex {};

// "Block Texture Atlas"
//
//     {
//       <int corresponding to Bedrock block type> => {
//         north: {
//           texture_index: 0,
//           path: "./assets/bedrock.png",
//         },
//         ...
//         bottom: {
//           texture_index: 0,
//           path: "./assets/bedrock.png",
//         },
//       },
//       ...
//       <int corresponding to Grass block type> => {
//         north: {
//           texture_index: 1,
//           path: "./assets/grass-nsew.png",
//         },
//         ...
//         bottom: {
//           texture_index: 3,
//           path: "./assets/grass-bottom.png",
//         },
//       },
//       ...
//     }

void Texture::loadBlockTextures() {
  // Initialize the Texture RubyVM and create the block texture atlas.
  if (mrb_nil_p(s_mrbBlockTextureAtlas)) {
    s_mrb = RubyVM::spawnVM();
    s_mrbLoadsBlockTextureAtlasResult = mrbext_load_and_check_string(s_mrb, "LoadsBlockTextureAtlas.call");
    s_mrbBlockTextureAtlas = mrb_hash_get(s_mrb, s_mrbLoadsBlockTextureAtlasResult, mrb_check_intern_cstr(s_mrb, "atlas"));
  }

  // Create a new texture array
  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);

  // Configure how many images in the array and the size of each image.
  int imageSize = 16;
  int textureCount = mrb_fixnum(mrb_hash_get(s_mrb, s_mrbLoadsBlockTextureAtlasResult, mrb_check_intern_cstr(s_mrb, "texture_count")));
  glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, imageSize, imageSize, textureCount);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
  mrb_hash_foreach(s_mrb, RHASH(s_mrbBlockTextureAtlas), [](mrb_state* mrb, mrb_value key, mrb_value value, void *data) -> int {
      mrb_value mrbSidesToTextureInfo = std::move(value);
      mrb_hash_foreach(s_mrb, RHASH(mrbSidesToTextureInfo), [](mrb_state* mrb, mrb_value key, mrb_value value, void *data) -> int {
          mrb_value mrbTextureInfo = std::move(value);

          mrb_value mrbTexturePath = mrb_hash_get(mrb, mrbTextureInfo, mrb_check_intern_cstr(mrb, "path"));
          std::string texturePath = mrb_string_cstr(mrb, mrbTexturePath);

          mrb_value mrbTextureIndex = mrb_hash_get(mrb, mrbTextureInfo, mrb_check_intern_cstr(mrb, "texture_index"));
          float textureIndex = mrb_fixnum(mrbTextureIndex);

          Texture::loadBlockImageIntoTexture(texturePath, textureIndex);
          return 0;
      }, (void*)0);
      return 0;
  }, (void*)0);
#pragma GCC diagnostic pop

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
  mrb_value mrbSideToTextureIndex = mrb_hash_get(s_mrb, s_mrbBlockTextureAtlas, mrb_fixnum_value(blockType));
  mrb_value mrbTextureInfo = mrb_hash_get(s_mrb, mrbSideToTextureIndex, mrbext_side_to_sym(s_mrb, side));
  mrb_value mrbTextureIndex = mrb_hash_get(s_mrb, mrbTextureInfo, mrb_check_intern_cstr(s_mrb, "texture_index"));
  float textureIndex = mrb_fixnum(mrbTextureIndex);
  return textureIndex;
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
