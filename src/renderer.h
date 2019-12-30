#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Renderer {
  private:
    float vertices[9] {
      -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };
    unsigned int vbo {};
    unsigned int vao {};
  public:
    Renderer();
    void render(double dt);
};
