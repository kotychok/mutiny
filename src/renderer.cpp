#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "renderer.h"

Renderer::Renderer() {
  std::cout << "Renderer created" << std::endl;
}

void Renderer::render(double dt) {
  glClearColor(0.2f, 0.3f, 0.3f, 0.1f);
  glClear(GL_COLOR_BUFFER_BIT);
}
