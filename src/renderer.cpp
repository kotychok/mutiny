#include <iostream>

#include "stb/stb_image.h"

#include "renderer.h"
#include "shader.h"
#include "texture.h"

Renderer::Renderer() {
  std::cout << "Renderer created" << std::endl;

  stbi_set_flip_vertically_on_load(true);

  glEnable(GL_DEPTH_TEST);

  // Axes
  glGenVertexArrays(1, &xAxisVAO);
  glGenBuffers(1, &xAxisVBO);

  glGenVertexArrays(1, &yAxisVAO);
  glGenBuffers(1, &yAxisVBO);

  glGenVertexArrays(1, &zAxisVAO);
  glGenBuffers(1, &zAxisVBO);

  // Cubes
  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &cubeVBO);

  glActiveTexture(GL_TEXTURE0);
  Texture containerTexture("./assets/dirt.jpg");
}

void Renderer::render(double dt) {
  glClearColor(0.2f * dt, 0.3f, 0.3f, 0.1f); // Use dt here for now to get rid of the warning while I fill out the rest of the missing code. It's a nice color.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // Cube stuff
  glBindVertexArray(cubeVAO);

  glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  blockShader.use();
  blockShader.setInt("myTexture", 0);
  blockShader.setMat4("view", camera.getViewMatrix());
  blockShader.setMat4("projection", camera.getProjectionMatrix());

  glm::mat4 chunkModel = glm::mat4(1.0f);
  for (const auto &iter : chunks) {
    auto chunk = iter.second;
    chunk.render(blockShader, chunkModel);
  }

  // Axes stuff
  glm::mat4 axisModel = glm::mat4(1.0f);
  lineShader.use();
  lineShader.setMat4("view", camera.getViewMatrix());
  lineShader.setMat4("projection", camera.getProjectionMatrix());
  lineShader.setMat4("model", axisModel);

  glBindVertexArray(xAxisVAO);
  glBindBuffer(GL_ARRAY_BUFFER, xAxisVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(xAxisVertices), xAxisVertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  lineShader.setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));
  glDrawArrays(GL_LINES, 0, 2);
  glDrawArrays(GL_POINTS, 0, 2);

  glBindVertexArray(yAxisVAO);
  glBindBuffer(GL_ARRAY_BUFFER, yAxisVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(yAxisVertices), yAxisVertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  lineShader.setVec3("color", glm::vec3(0.0f, 1.0f, 0.0f));
  glDrawArrays(GL_LINES, 0, 2);
  glDrawArrays(GL_POINTS, 0, 2);

  glBindVertexArray(zAxisVAO);
  glBindBuffer(GL_ARRAY_BUFFER, zAxisVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(zAxisVertices), zAxisVertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  lineShader.setVec3("color", glm::vec3(0.0f, 0.0f, 1.0f));
  glDrawArrays(GL_LINES, 0, 2);
  glDrawArrays(GL_POINTS, 0, 2);
}

void Renderer::processInput(GLFWwindow* window, float dt) {
  camera.processInput(window, dt);
}

void Renderer::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
  camera.mouseCallback(window, xpos, ypos);
}
