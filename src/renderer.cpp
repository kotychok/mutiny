#include <iostream>

#include "stb/stb_image.h"

#include "renderer.h"
#include "shader.h"
#include "texture.h"

Renderer::Renderer() {
  std::cout << "Renderer created" << std::endl;

  stbi_set_flip_vertically_on_load(true);

  glEnable(GL_DEPTH_TEST);

  glGenBuffers(1, &vbo);
  glGenVertexArrays(1, &vao);

  glActiveTexture(GL_TEXTURE0);
  Texture containerTexture("./assets/dirt.jpg");

  myShader.use();
  myShader.setInt("myTexture", 0);
}

void Renderer::render(double dt) {
  glClearColor(0.2f * dt, 0.3f, 0.3f, 0.1f); // Use dt here for now to get rid of the warning while I fill out the rest of the missing code. It's a nice color.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  myShader.setMat4("view", camera.getViewMatrix());
  myShader.setMat4("projection", camera.getProjectionMatrix());

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glm::mat4 model = glm::mat4(1.0f);
  myChunk.render(&myShader, model);
}

void Renderer::processInput(GLFWwindow* window, float dt) {
  camera.processInput(window, dt);
}

void Renderer::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
  camera.mouseCallback(window, xpos, ypos);
}
