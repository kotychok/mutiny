#include <cmath>

#include <glm/gtx/scalar_multiplication.hpp>

#include "chunk.h"

Chunk::Chunk(const Chunk &chunk) : pos{chunk.pos} {
    std::cout << "Chunk copy constructor called\n";
}

Chunk::Chunk(glm::vec3 pos) : pos{pos} {
  std::cout << "Chunk at " << pos.x << ", " << pos.y << ", " << pos.z << " created" << std::endl;
  int radius = 8;
  for (unsigned int x = 0; x < Chunk::SIZE; x++) {
    for (unsigned int z = 0; z < Chunk::SIZE; z++) {
      for (unsigned int y = 0; y < Chunk::SIZE; y++) {
        if (y < 8) {
          unsigned int blockX = x - Chunk::SIZE / 2;
          unsigned int blockY = y - Chunk::SIZE / 2;
          unsigned int blockZ = z - Chunk::SIZE / 2;
          if (sqrt(blockX * blockX + blockY * blockY + blockZ * blockZ) < radius) {
            unsigned int index { z * SIZE * SIZE + y * SIZE + x };
            blocks[index] = 1;
          }
        }
      }
    }
  }
}

Chunk::~Chunk() {
  std::cout << "Chunk at " << pos.x << ", " << pos.y << ", " << pos.z << " destroyed" << std::endl;
}

void Chunk::render(const Shader &myShader, const glm::mat4 &chunkModel) {
  for (unsigned int x = 0; x < Chunk::SIZE; x++) {
    for (unsigned int z = 0; z < Chunk::SIZE; z++) {
      for (unsigned int y = 0; y < Chunk::SIZE; y++) {
        unsigned int index { z * SIZE * SIZE + y * SIZE + x };
        if (blocks[index] == 1) {
          glm::mat4 blockModel = glm::translate(chunkModel, glm::vec3(x, y, z));
          blockModel = glm::translate(blockModel, glm::vec3(0.5f, 0.5f, 0.5f));
          blockModel = glm::translate(blockModel, pos * Chunk::SIZE);
          blockModel = glm::translate(blockModel, -glm::vec3(Chunk::SIZE / 2, 0, Chunk::SIZE / 2));
          myShader.setMat4("model", blockModel);
          glDrawArrays(GL_TRIANGLES, 0, 36);
        }
      }
    }
  }
}
