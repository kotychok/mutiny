#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "renderer.h"
#include "window.h"

void errorCallback(int error, const char* description) {
  std::cerr << "Error: " << error << ", Description: " << description << std::endl;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  // std::cout
    // << "key: " << key
    // << " scancode: " << scancode
    // << " action: " << action
    // << " mods: " << mods << std::endl;
  if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  // std::cout << "xoffset: " << xoffset << " yoffset: " << yoffset << std::endl;
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
  Renderer* rendererPtr = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
  rendererPtr->mouseCallback(window, xpos, ypos);
  // std::cout << "xpos: " << xpos << " ypos: " << ypos << std::endl;
}
#pragma GCC diagnostic pop

Window::Window() {
  std::cout << "Window created" << std::endl;
}

int Window::show() {
  glfwSetErrorCallback(errorCallback);

  glfwInit();
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "Mutiny :)", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glfwSetCursorPosCallback(window, mouseCallback);
  glfwSetKeyCallback(window, keyCallback);
  glfwSetScrollCallback(window, scrollCallback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  Renderer renderer;
  glfwSetWindowUserPointer(window, &renderer);

  glfwSetTime(0.0);
  double lastTime = 0.0;
  while(!glfwWindowShouldClose(window)) {
    double time = glfwGetTime();
    double dt = time - lastTime;
    lastTime = time;

    glfwPollEvents();

    renderer.processInput(window, dt);
    renderer.render(dt);

    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}
