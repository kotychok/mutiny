#pragma once

#include <iostream>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "window.h"

class Camera {
  private:
    glm::vec3 cameraUp { glm::vec3(0.0f, 1.0f,  0.0f) };
    float yaw { 270.0f };
    float pitch { 0.0f };
    float lastX {};
    float lastY {};
    bool firstMouseMovement { true };
    int width { Window::WIDTH };
    int height { Window::HEIGHT };

  public:
    static constexpr float MIN_FOV { 22.5f };
    static constexpr float MAX_FOV { 67.5f };

    static constexpr float MIN_NEAR_PLANE { 0.01f };
    static constexpr float MAX_NEAR_PLANE { 10.0f };

    static constexpr float MIN_FAR_PLANE { 0.01f };
    static constexpr float MAX_FAR_PLANE { 1000.0f };

    float fov { 45.0f };
    float nearPlane { 0.1f };
    float farPlane { 400.0f }; // This is a tiny bit over enough to see 10 chunks out.

    glm::vec3 position { glm::vec3(0.0f, 15.0f,  0.0f) };

    glm::vec3 cameraFront() {
      glm::vec3 direction {};
      direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
      direction.y = sin(glm::radians(pitch));
      direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
      return glm::normalize(direction);
    };

    glm::mat4 getViewMatrix() {
      return glm::lookAt(position, position + cameraFront(), cameraUp);
    };

    glm::mat4 getProjectionMatrix() {
      return glm::perspective(glm::radians(fov), static_cast<float>(width) / height, nearPlane, farPlane);
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
        position -= cameraSpeed * glm::normalize(glm::cross(cameraFront(), cameraUp));
      }
      if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position += cameraSpeed * glm::normalize(glm::cross(cameraFront(), cameraUp));
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
    void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
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
    };

    void windowSizeCallback(GLFWwindow* window, int width, int height) {
      this->width = width;
      this->height = height;
    };
#pragma GCC diagnostic pop
};
