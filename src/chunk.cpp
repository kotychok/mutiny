#include <glm/gtx/scalar_multiplication.hpp>

#include "chunk.h"

const int Chunk::SIZE { 16 };

Chunk::Chunk(glm::vec3 pos) : pos{pos} {
}

void Chunk::render(const Shader &myShader, const glm::mat4 &chunkModel) {
  for (unsigned int x = 0; x < Chunk::SIZE; x++) {
    for (unsigned int z = 0; z < Chunk::SIZE; z++) {
      for (unsigned int y = 0; y < Chunk::SIZE; y++) {
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
