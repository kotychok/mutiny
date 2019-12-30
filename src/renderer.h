#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Renderer {
  private:
    float vertices[32] {
      // positions          // colors           // texture coords
       0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
       0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
      -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
      -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
    };
    unsigned int indices[6] {
      0, 1, 3,
      1, 2, 3,
    };
    unsigned int vbo {};
    unsigned int vao {};
    unsigned int ebo {};
  public:
    Renderer();
    void render(double dt);
};
