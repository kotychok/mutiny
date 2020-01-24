#include <array>
#include <algorithm>
#include <vector>

#include <glm/gtx/scalar_multiplication.hpp>

#include "block.h"
#include "chunk.h"
#include "chunk_generator.h"
#include "texture.h"

Chunk::Chunk(glm::vec3 pos, std::function<std::array<Block, CHUNK_SIZE_CUBED>()> chunkGeneratorFunc) : pos{pos}, chunkGeneratorFunc{chunkGeneratorFunc} {
  blocks = chunkGeneratorFunc();
}

Chunk::~Chunk() {
}

void Chunk::setMesh(std::vector<quad_mesh> quadMeshes) {
  this->quadMeshes = quadMeshes;

  if (!chunkVAO) {
    glGenVertexArrays(1, &chunkVAO);
  }
  if (!chunkVBO) {
    glGenBuffers(1, &chunkVBO);
  }

  for (quad_mesh quadMesh : quadMeshes) {
    for (float f : quadMesh) {
      flat.push_back(f);
    }
  }

  glBindVertexArray(chunkVAO);
  glBindBuffer(GL_ARRAY_BUFFER, chunkVBO);
  glBufferData(GL_ARRAY_BUFFER, static_cast<float>(flat.size()) * sizeof(float), flat.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
}

bool Chunk::isBlockAt(unsigned int x, unsigned int y, unsigned int z) const {
  return blockAt(x, y, z).type != BlockType::EMPTY;
}

Block Chunk::blockAt(unsigned int x, unsigned int y, unsigned int z) const {
  if (x > CHUNK_SIZE - 1)
    throw std::invalid_argument("X is larger than CHUNK_SIZE - 1");
  if (y > CHUNK_SIZE - 1)
    throw std::invalid_argument("Y is larger than CHUNK_SIZE - 1");
  if (z > CHUNK_SIZE - 1)
    throw std::invalid_argument("Z is larger than CHUNK_SIZE - 1");

  unsigned int index { z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x };
  return blocks.at(index);
}

void Chunk::render(const Shader &myShader) {
  glm::mat4 blockModel = glm::mat4(1.0);
  blockModel = glm::translate(blockModel, pos * CHUNK_SIZE);
  myShader.setMat4("model", blockModel);

  glBindVertexArray(chunkVAO);
  glDrawArrays(GL_TRIANGLES, 0, flat.size() / 6);
}
