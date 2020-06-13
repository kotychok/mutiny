#include <array>
#include <algorithm>
#include <vector>

#include <glm/gtx/scalar_multiplication.hpp>
#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/array.h>
#include <mruby/string.h>

#include "block.h"
#include "chunk.h"
#include "chunk_generator.h"
#include "texture.h"

Chunk::Chunk(glm::vec3 pos, std::shared_ptr<mrb_state> mrb, std::string chunkGeneratorFunc) : pos{pos}, m_mrb{mrb}, chunkGeneratorFunc{chunkGeneratorFunc} {
  // blocks = chunkGeneratorFunc(pos);

  mrb_value testValue = mrb_load_string(m_mrb.get(), "ChunkGenerator.flat");
  RArray* myArray = RARRAY(testValue);
  std::cout << myArray->as.heap.len << std::endl;
  mrb_value firstElement = mrb_ary_ref(m_mrb.get(), testValue, 0);
  std::cout << mrb_string_p(firstElement) << std::endl;
  mrb_p(m_mrb.get(), firstElement);
  const char* myCStr = mrb_string_cstr(m_mrb.get(), firstElement);
  std::cout << myCStr << std::endl;
}

Chunk::~Chunk() {
}

void Chunk::setMesh(std::vector<float> mesh) {
  this->mesh = mesh;
  this->isMeshDirty = true;
}

bool Chunk::isBlockAt(unsigned int x, unsigned int y, unsigned int z) const {
  return blockAt(x, y, z) != Block::EMPTY_BLOCK;
}

WorldBlock Chunk::blockAt(unsigned int x, unsigned int y, unsigned int z) const {
  if (x > CHUNK_SIZE - 1)
    throw std::invalid_argument("X is larger than CHUNK_SIZE - 1");
  if (y > CHUNK_SIZE - 1)
    throw std::invalid_argument("Y is larger than CHUNK_SIZE - 1");
  if (z > CHUNK_SIZE - 1)
    throw std::invalid_argument("Z is larger than CHUNK_SIZE - 1");

  unsigned int index { z * CHUNK_SIZE_SQUARED + y * CHUNK_SIZE + x };
  return blocks.at(index);
}

void Chunk::render(const Shader &shader) {
  if (!chunkVAO) {
    glGenVertexArrays(1, &chunkVAO);
  }
  if (!chunkVBO) {
    glGenBuffers(1, &chunkVBO);
  }

  if (isMeshDirty) {
    glBindVertexArray(chunkVAO);
    glBindBuffer(GL_ARRAY_BUFFER, chunkVBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<float>(mesh.size()) * sizeof(float), mesh.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    isMeshDirty = false;
  }

  // Only render the chunk if the mesh is up to date. I'm not sure how
  // necessary this is, but it just seems like a pretty good idea to not render
  // the old mesh if we know it's not correct anymore. My hunch is that this
  // probably doesn't matter most of the time but I'm not sure.
  if (!isMeshDirty) {
    glm::mat4 blockModel = glm::mat4(1.0);
    blockModel = glm::translate(blockModel, pos * CHUNK_SIZE);
    shader.setMat4("model", blockModel);

    glBindVertexArray(chunkVAO);
    glDrawArrays(GL_TRIANGLES, 0, mesh.size() / 6);
  }
}
