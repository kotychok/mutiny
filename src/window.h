#pragma once

#include <GLFW/glfw3.h>

class Window {
  public:
    static constexpr int WIDTH { 800 };
    static constexpr int HEIGHT { 600 };

    static void errorCallback(int error, const char* description);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void windowSizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);

    static bool isFocusedInGame(GLFWwindow* window);
    static bool isFocusedInGUI(GLFWwindow* window);
    static void focusInGame(GLFWwindow* window);
    static void focusInGUI(GLFWwindow* window);

    Window();
    int show();
};
