#pragma once

#include <glad/glad.h>

#include <iostream>

#include "stb/stb_image.h"

class Texture {
  public:
    unsigned int ID {};

    Texture(const char* imagePath) {
      glGenTextures(1, &ID);
      glBindTexture(GL_TEXTURE_2D, ID);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      int width, height, nrChannels;
      unsigned char *data = stbi_load(imagePath, &width, &height, &nrChannels, 0);
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
    };
};
