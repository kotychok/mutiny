#pragma once

#include "shader.h"

class Chunk {
  public:
    Chunk();
    void render(Shader* myShader, glm::mat4 &chunkModel);
  private:
};
