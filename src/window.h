#pragma once

#include <GLFW/glfw3.h>

class Window {
  public:
    static constexpr int WIDTH { 800 };
    static constexpr int HEIGHT { 600 };

    static bool isFocusedInGame(GLFWwindow* window);
    static bool isFocusedInGUI(GLFWwindow* window);
    static void focusInGame(GLFWwindow* window);
    static void focusInGUI(GLFWwindow* window);

    Window();
    int show();
};
