#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
  public:
    glm::mat4 getViewMatrix() {
      return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    };

    glm::mat4 getProjectionMatrix() {
      // TODO Don't hardcode height & width
      return glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    };

    void processInput(GLFWwindow* window, float dt) {
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
      if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        cameraPos -= cameraSpeed * glm::vec3(0.0f, 1.0f, 0.0f);
      }
      if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        cameraPos += cameraSpeed * glm::vec3(0.0f, 1.0f, 0.0f);
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
#pragma GCC diagnostic pop

  private:
    glm::vec3 cameraPos { glm::vec3(0.0f, 0.0f,  20.0f) };
    glm::vec3 cameraFront { glm::vec3(0.0f, -1.0f, -1.0f) };
    glm::vec3 cameraUp { glm::vec3(0.0f, 1.0f,  0.0f) };
    float yaw { 270.0f };
    float pitch { 0.0f };
    float lastX {};
    float lastY {};
    bool firstMouseMovement { true };
};
