#include <array>
#include <algorithm>
#include <cmath>
#include <vector>

#include <glm/gtx/scalar_multiplication.hpp>

#include "chunk.h"
#include "mesher_greedy.h"

Chunk::Chunk(glm::vec3 pos) : pos{pos} {
  // Cubes
  glGenVertexArrays(1, &chunkVAO);
  glGenBuffers(1, &chunkVBO);

  if (pos.y == 0) {
    for (unsigned int x = 0; x < Chunk::SIZE; x++) {
      for (unsigned int z = 0; z < Chunk::SIZE; z++) {
        unsigned int index { z * SIZE * SIZE + 0 * SIZE + x };
        blocks[index] = 1;
      }
    }

    // int radius = 8;
    // for (unsigned int x = 0; x < Chunk::SIZE; x++) {
      // for (unsigned int z = 0; z < Chunk::SIZE; z++) {
        // for (unsigned int y = 0; y < Chunk::SIZE; y++) {
          // if (y < 8) {
            // unsigned int blockX = x - Chunk::SIZE / 2;
            // unsigned int blockY = y - Chunk::SIZE / 2;
            // unsigned int blockZ = z - Chunk::SIZE / 2;
            // if (sqrt(blockX * blockX + blockY * blockY + blockZ * blockZ) < radius) {
              // unsigned int index { z * SIZE * SIZE + y * SIZE + x };
              // blocks[index] = 1;
            // }
          // }
        // }
      // }
    // }
  }

  quads = MesherGreedy::chunkToQuads(*this);
  transform(quads.begin(), quads.end(), back_inserter(quadMeshes), MesherGreedy::quadToQuadMesh);
}

Chunk::~Chunk() {
}

bool Chunk::isBlockAt(int x, int y, int z) const {
  int index { z * SIZE * SIZE + y * SIZE + x };
  return blocks[index] == 1;
}

void Chunk::render(const Shader &myShader) {
  for (quad_mesh quadMesh : quadMeshes) {
    glBindVertexArray(chunkVAO);

    glBindBuffer(GL_ARRAY_BUFFER, chunkVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadMesh), quadMesh.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glm::mat4 blockModel = glm::mat4(1.0);
    blockModel = glm::translate(blockModel, pos * Chunk::SIZE);
    myShader.setMat4("model", blockModel);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
}
