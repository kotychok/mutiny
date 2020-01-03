#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

class Renderer {
  public:
    Renderer();
    void render(double dt);
};

class Window {
  public:
    Window();
    int show();
};

Window::Window() {
  std::cout << "Window created" << std::endl;
}

Renderer renderer;

int Window::show() {
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

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glViewport(0, 0, 800, 600);

  glfwSetTime(0.0);
  double last_time = 0.0;
  while(!glfwWindowShouldClose(window)) {
    double time = glfwGetTime();
    double dt = time - last_time;
    last_time = time;

    glfwPollEvents();

    renderer.render(dt);

    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}

Renderer::Renderer() {
  std::cout << "Renderer created" << std::endl;

  // The segfault is because I'm calling an OpenGL method before creating an
  // OpenGL context.
  glEnable(GL_DEPTH_TEST);
}

void Renderer::render(double dt) {
  glClearColor(0.2f * dt, 0.3f, 0.3f, 0.1f); // Use dt here for now to get rid of the warning while I fill out the rest of the missing code. It's a nice color.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int main() {
  Window window;
  return window.show();
}
