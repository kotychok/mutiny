#include <array>
#include <algorithm>
#include <vector>

#include <glm/gtx/scalar_multiplication.hpp>

#include "block.h"
#include "chunk.h"
#include "chunk_generator.h"
#include "texture.h"

Chunk::Chunk(glm::vec3 pos) : pos{pos} {
  // blocks = ChunkGenerator::halfSphere();
  blocks = ChunkGenerator::flat();
}

Chunk::~Chunk() {
}

void Chunk::setMesh(std::vector<quad_mesh> quadMeshes) {
  this->quadMeshes = quadMeshes;
}

bool Chunk::isBlockAt(unsigned int x, unsigned int y, unsigned int z) const {
  unsigned int index { z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x };
  return blocks.at(index).type != BlockType::AIR;
}

void Chunk::render(const Shader &myShader) {
  if (!chunkVAO) {
    glGenVertexArrays(1, &chunkVAO);
  }
  if (!chunkVBO) {
    glGenBuffers(1, &chunkVBO);
  }

  for (quad_mesh quadMesh : quadMeshes) {
    glBindVertexArray(chunkVAO);

    glBindBuffer(GL_ARRAY_BUFFER, chunkVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadMesh), quadMesh.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glm::mat4 blockModel = glm::mat4(1.0);
    blockModel = glm::translate(blockModel, pos * CHUNK_SIZE);
    myShader.setMat4("model", blockModel);

    Texture& dirt = Texture::fetch(BlockType::DIRT);
    // Texture& bedrock = Texture::fetch(BlockType::BEDROCK);
    glBindTexture(GL_TEXTURE_2D, dirt.ID);

    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
}
