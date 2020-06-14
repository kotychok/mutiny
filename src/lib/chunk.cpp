#include <mruby/compile.h>
#include <mruby/array.h>
#include <mruby/string.h>

#include "chunk.h"
#include "ruby_vm.h"

Chunk::Chunk(glm::vec3 pos, std::string chunkGeneratorFunc) : pos{pos}, chunkGeneratorFunc{chunkGeneratorFunc} {
}

Chunk::~Chunk() {
}

void Chunk::generate() {
  mrb_state *mrb = RubyVM::spawnVM();

  std::string code = chunkGeneratorFunc + "(" + std::to_string(static_cast<int>(pos.x)) + "," + std::to_string(static_cast<int>(pos.y)) + "," + std::to_string(static_cast<int>(pos.z)) + ")";
  const char *c_str = code.c_str();

  // TODO Make a wrapper object that handles error checking.
  mrb_value mrbBlocks = mrb_load_string(mrb, c_str);
  if (mrb->exc) { // If there is an error
    if (!mrb_undef_p(mrbBlocks)) {
      mrb_print_error(mrb); // print exception object
    }
  }

  // FIXME I definitely get lag when crossing chunk boundary,
  // and I'm gonna make a wild guess it's from copying this array.
  // I bet if I could somehow prevent copying/looping these extra times that
  // might go a long way to solving it.
  for (unsigned int i = 0; i < RARRAY_LEN(mrbBlocks); ++i) {
    mrb_value element = mrb_ary_ref(mrb, mrbBlocks, i);
    if (mrb_string_p(element)) {
      std::string blockId = mrb_string_cstr(mrb, element);
      blocks[i] = Block::createWorldBlock(blockId);
    }
  }

  mrb_close(mrb);
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
