#include <cmath>

#include <glm/gtx/scalar_multiplication.hpp>

#include "chunk.h"

Chunk::Chunk(glm::vec3 pos) : pos{pos} {
  std::cout << "Chunk " << this << " at " << pos.x << ", " << pos.y << ", " << pos.z << " created" << std::endl;
  if (pos.y > 0) {
    return;
  }
  for (unsigned int x = 0; x < Chunk::SIZE; x++) {
    for (unsigned int z = 0; z < Chunk::SIZE; z++) {
      unsigned int index { z * SIZE * SIZE + 0 * SIZE + x };
      blocks[index] = 1;
    }
  }
}

Chunk::~Chunk() {
  std::cout << "Chunk " << this << " at " << pos.x << ", " << pos.y << ", " << pos.z << " destroyed" << std::endl;
}

void Chunk::render(const Shader &myShader) {
  for (unsigned int x = 0; x < Chunk::SIZE; x++) {
    for (unsigned int z = 0; z < Chunk::SIZE; z++) {
      for (unsigned int y = 0; y < Chunk::SIZE; y++) {
        unsigned int index { z * SIZE * SIZE + y * SIZE + x };
        if (blocks[index] == 1) {
          glm::mat4 blockModel = glm::translate(glm::mat4(1.0), glm::vec3(x, y, z));
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
