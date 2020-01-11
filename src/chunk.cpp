#include <cmath>

#include <glm/gtx/scalar_multiplication.hpp>

#include "chunk.h"

Chunk::Chunk(glm::vec3 pos) : pos{pos} {
  std::cout << "Chunk " << this << " at " << pos.x << ", " << pos.y << ", " << pos.z << " created" << std::endl;

  // Cubes
  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &chunkVBO);
  // glGenBuffers(1, &chunkEBO);

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
  // float chunkVertices[24] =  {
    // 8.0f, 1.0f,  8.0f,   // right-top-front corner
    // 8.0f, 1.0f, -8.0f,   // right-top-back corner
    // -8.0f, 1.0f, -8.0f,   // left-top-back corner
    // -8.0f, 1.0f,  8.0f,   // left-top-front corner

    // -8.0f, 0.0f, -8.0f,   // left-bottom-back corner
    // 8.0f, 0.0f, -8.0f,   // right-bottom-back corner
    // -8.0f, 0.0f,  8.0f,   // left-bottom-front corner
    // 8.0f, 0.0f,  8.0f,   // left-bottom-front corner
  // };

  float chunkVertices[] =  {
     8.0f, 1.0f,  8.0f,  8.0f,  8.0f,   // right-top-front corner
     8.0f, 1.0f, -8.0f,  8.0f, -8.0f,  // right-top-back corner
    -8.0f, 1.0f,  8.0f, -8.0f,  8.0f,  // left-top-front corner

     8.0f, 1.0f, -8.0f,  8.0f, -8.0f,  // right-top-back corner
    -8.0f, 1.0f, -8.0f, -8.0f, -8.0f,  // left-top-back corner
    -8.0f, 1.0f,  8.0f, -8.0f,  8.0f,  // left-top-front corner
  };

  // float chunkIndices[] = {
    // 0, 1, 2,
    // 1, 4, 2,
  // };

  glBindVertexArray(cubeVAO);

  glBindBuffer(GL_ARRAY_BUFFER, chunkVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(chunkVertices), chunkVertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunkEBO);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(chunkIndices), chunkIndices, GL_STATIC_DRAW);

  // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  // glEnableVertexAttribArray(1);

  glm::mat4 blockModel = glm::mat4(1.0);
  blockModel = glm::translate(blockModel, pos * Chunk::SIZE);
  myShader.setMat4("model", blockModel);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
