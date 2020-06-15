#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::vec3 Camera::computeCameraFront() {
  glm::vec3 direction {};
  direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  return glm::normalize(direction);
}

glm::mat4 Camera::getViewMatrix() const {
  return glm::lookAt(position, position + cameraFront, cameraUp);
}

glm::mat4 Camera::getProjectionMatrix() const {
  return glm::perspective(glm::radians(fov), static_cast<float>(width) / height, nearPlane, farPlane);
}

void Camera::processInput(GLFWwindow* window, float dt) {
  float cameraSpeed = speed * dt;
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
    position -= cameraSpeed * cameraUp;
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    position += cameraSpeed * cameraUp;
  }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void Camera::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
  if (!lastX) {
    lastX = xpos;
  }
  if (!lastY) {
    lastY = ypos;
  }
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;

  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if (pitch > 89.0f) {
    pitch = 89.0f;
  } else if (pitch < -89.0f) {
    pitch = -89.0f;
  }

  cameraFront = computeCameraFront();
}

void Camera::windowSizeCallback(GLFWwindow* window, int width, int height) {
  this->width = width;
  this->height = height;
}

void Camera::focusCallback(bool focusedInGame) {
  if (focusedInGame) {
    // Clear out lastX and lastY so that they are re-initialized to the new
    // initial xpos and ypos in cursorPosCallback when going from GUI focus
    // mode to game focus mode.
    lastX = 0;
    lastY = 0;
  }
}
#pragma GCC diagnostic pop
