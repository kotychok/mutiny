#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "stb/stb_image.h"

#include "renderer.h"
#include "shader.h"
#include "texture.h"

Renderer::Renderer() {
  std::cout << "Renderer created" << std::endl;

  stbi_set_flip_vertically_on_load(true);

  glGenBuffers(1, &vbo);
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &ebo);

  // 1st Texture
  glActiveTexture(GL_TEXTURE0);
  Texture containerTexture("./assets/container.jpg");

  // 2nd texture
  glActiveTexture(GL_TEXTURE1);
  Texture awesomefaceTexture("./assets/awesomeface.png");

  Shader myShader("./src/shaders/vertex.vert", "./src/shaders/fragment.frag");
  myShader.use();
  myShader.setInt("ourTexture", 0);
  myShader.setInt("texture2", 1);
}

void Renderer::render(double dt) {
  glClearColor(0.2f * dt, 0.3f, 0.3f, 0.1f); // Use dt here for now to get rid of the warning while I fill out the rest of the missing code. It's a nice color.
  glClear(GL_COLOR_BUFFER_BIT);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
