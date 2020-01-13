#pragma once

#include <iostream>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "window.h"

class Camera {
  private:
    glm::vec3 cameraFront { glm::vec3(0.0f, -1.0f, -1.0f) };
    glm::vec3 cameraUp { glm::vec3(0.0f, 1.0f,  0.0f) };
    float yaw { 270.0f };
    float pitch { 0.0f };
    float lastX {};
    float lastY {};
    bool firstMouseMovement { true };
    int width { Window::WIDTH };
    int height { Window::HEIGHT };

  public:
    glm::vec3 position { glm::vec3(0.0f, 15.0f,  0.0f) };

    glm::mat4 getViewMatrix() {
      return glm::lookAt(position, position + cameraFront, cameraUp);
    };

    glm::mat4 getProjectionMatrix() {
      return glm::perspective(glm::radians(45.0f), static_cast<float>(width) / height, 0.1f, 100.0f);
    };

    void processInput(GLFWwindow* window, float dt) {
      float cameraSpeed = 7.0f * dt;
      if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position.x += cameraSpeed * cos(glm::radians(yaw));
        position.z += cameraSpeed * sin(glm::radians(yaw));
      }
      if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position.x -= cameraSpeed * cos(glm::radians(yaw));
        position.z -= cameraSpeed * sin(glm::radians(yaw));
      }
      if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position -= cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
      }
      if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position += cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
      }
      if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        position -= cameraSpeed * glm::vec3(0.0f, 1.0f, 0.0f);
      }
      if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        position += cameraSpeed * glm::vec3(0.0f, 1.0f, 0.0f);
      }
    };

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
    void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
      if (firstMouseMovement) {
        lastX = xpos;
        lastY = ypos;
        firstMouseMovement = false;
      }
      float xoffset = xpos - lastX;
      float yoffset = lastY - ypos;
      lastX = xpos;
      lastY = ypos;

      const float sensitivity = 0.05f;
      xoffset *= sensitivity;
      yoffset *= sensitivity;

      yaw += xoffset;
      pitch += yoffset;

      if (pitch > 89.0f) {
        pitch = 89.0f;
      } else if (pitch < -89.0f) {
        pitch = -89.0f;
      }

      glm::vec3 direction;
      direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
      direction.y = sin(glm::radians(pitch));
      direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
      cameraFront = glm::normalize(direction);
    };

    void windowSizeCallback(GLFWwindow* window, int width, int height) {
      this->width = width;
      this->height = height;
    };
#pragma GCC diagnostic pop
};
