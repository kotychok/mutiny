#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void error_callback(int error, const char* description) {
  std::cerr << "Error: " << error << ", Description: " << description << std::endl;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  std::cout
    << "key: " << key
    << " scancode: " << scancode
    << " action: " << action
    << " mods: " << mods << std::endl;
  if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  std::cout << "xoffset: " << xoffset << " yoffset: " << yoffset << std::endl;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
  std::cout << "xpos: " << xpos << " ypos: " << ypos << std::endl;
}
#pragma GCC diagnostic pop

class Window {
  public:
    Window() {
      std::cout << "Window created" << std::endl;
    }

    int show() {
      glfwSetErrorCallback(error_callback);

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
      glfwSetKeyCallback(window, key_callback);
      glfwSetScrollCallback(window, scroll_callback);
      glfwSetCursorPosCallback(window, mouse_callback);

      if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
      }

      glViewport(0, 0, 800, 600);
      glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

      glfwSetTime(0.0);
      double last_time = 0.0;
      while(!glfwWindowShouldClose(window)) {
        double time = glfwGetTime();
        double dt = time - last_time;
        last_time = time;
        std::cout << "dt: " << dt << std::endl;

        glfwSwapBuffers(window);
        glfwPollEvents();
      }

      glfwTerminate();
      return 0;
    }
};

int main() {
  Window* window = new Window();
  return window->show();
}
