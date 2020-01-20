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

void Chunk::setMesh(std::vector<std::pair<quad_mesh, BlockType>> quadMeshes) {
  this->quadMeshes = quadMeshes;
}

bool Chunk::isBlockAt(unsigned int x, unsigned int y, unsigned int z) const {
  unsigned int index { z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x };
  return blocks.at(index).type != BlockType::EMPTY;
}

void Chunk::render(const Shader &myShader) {
  if (!chunkVAO) {
    glGenVertexArrays(1, &chunkVAO);
  }
  if (!chunkVBO) {
    glGenBuffers(1, &chunkVBO);
  }

  for (std::pair<quad_mesh, BlockType> pair : quadMeshes) {
    quad_mesh& quadMesh { pair.first };
    BlockType& blockType { pair.second };

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

    Texture& texture = Texture::fetch(blockType);
    glBindTexture(GL_TEXTURE_2D, texture.ID);

    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
}
