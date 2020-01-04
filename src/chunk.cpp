#include <glm/gtx/scalar_multiplication.hpp>

#include "chunk.h"

const int Chunk::SIZE { 16 };

Chunk::Chunk(glm::vec3 pos) : pos{pos} {
}

void Chunk::render(const Shader &myShader, const glm::mat4 &chunkModel) {
  for (unsigned int x = 0; x < 10; x++) {
    for (unsigned int z = 0; z < 10; z++) {
      glm::mat4 blockModel = glm::translate(chunkModel, glm::vec3(x, 0, z));
      blockModel = glm::translate(blockModel, pos * Chunk::SIZE);
      myShader.setMat4("model", blockModel);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
  }
}
