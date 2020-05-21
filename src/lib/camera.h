#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "window.h"

class Camera {
  private:
    float yaw { 270.0f };
    float pitch { 0.0f };
    glm::vec3 cameraFront { computeCameraFront() };
    glm::vec3 cameraUp { glm::vec3(0.0f, 1.0f,  0.0f) };
    int width { Window::WIDTH };
    int height { Window::HEIGHT };
    double lastX {};
    double lastY {};

  public:
    static constexpr float MIN_FOV { 22.5f };
    static constexpr float MAX_FOV { 67.5f };

    static constexpr float MIN_NEAR_PLANE { 0.01f };
    static constexpr float MAX_NEAR_PLANE { 10.0f };

    static constexpr float MIN_FAR_PLANE { 0.01f };
    static constexpr float MAX_FAR_PLANE { 1000.0f };

    static constexpr float MIN_SPEED { 1.0f };
    static constexpr float MAX_SPEED { 32.0f };

    static constexpr float MIN_SENSITIVITY { 0.01f };
    static constexpr float MAX_SENSITIVITY { 1.0f };

    float fov { 45.0f };
    float nearPlane { 0.1f };
    float farPlane { 400.0f }; // This is a tiny bit over enough to see 10 chunks out.
    float speed { 24.0f };
    float sensitivity { 0.05f };

    glm::vec3 position { glm::vec3(0.0f, 94.0f, 0.0f) };

    glm::vec3 computeCameraFront();

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;

    void processInput(GLFWwindow* window, float dt);
    void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    void windowSizeCallback(GLFWwindow* window, int width, int height);
    void focusCallback(bool focusedInGame);
};
