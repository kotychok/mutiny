#include "chunk.h"

Chunk::Chunk() {
}

void Chunk::render(Shader* myShader, glm::mat4 &chunkModel) {
  for (unsigned int x = 0; x < 10; x++) {
    for (unsigned int z = 0; z < 10; z++) {
      glm::mat4 blockModel = glm::translate(chunkModel, glm::vec3(x, 0, z));
      myShader->setMat4("model", blockModel);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
  }
}
