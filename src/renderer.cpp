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

  // 1st Texture
  glActiveTexture(GL_TEXTURE0);
  Texture containerTexture("./assets/container.jpg");

  // 2nd texture
  glActiveTexture(GL_TEXTURE1);
  Texture awesomefaceTexture("./assets/awesomeface.png");

  myShader.use();
  myShader.setInt("ourTexture", 0);
  myShader.setInt("texture2", 1);
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

  glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

  glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

  myShader.setMat4("view", view);
  myShader.setMat4("projection", projection);

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  for (unsigned int i = 0; i < 10; i++) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, cubePositions[i]);
    float angle = 20.0f * i;
    if (i % 3 == 0) {
      angle *= glfwGetTime();
    }
    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
    myShader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }
}

void Renderer::processInput(GLFWwindow* window, float dt) {
  float cameraSpeed = 2.5f * dt;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    cameraPos += cameraSpeed * cameraFront;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    cameraPos -= cameraSpeed * cameraFront;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    cameraPos -= cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    cameraPos += cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
  }
}
