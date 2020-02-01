#pragma once

#include "map_with_tuple_keys.h"

#include <tuple>
#include <unordered_map>
#include <unordered_set>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/scalar_multiplication.hpp>

#include "camera.h"
#include "chunk.h"
#include "shader.h"

typedef std::tuple<int, int, int> xyz;

const float DIRECTIONAL_LIGHT = 0.0f;
const float POINT_LIGHT = 1.0f;

struct SunMoon {
  float angleInDegrees;
  glm::vec3 color;
  float brightness;

  glm::vec4 direction() {
    float angleInRadians = glm::radians(angleInDegrees);
    float sunMoonX = -cos(angleInRadians);
    float sunMoonY = -sin(angleInRadians);
    return glm::vec4(sunMoonX, sunMoonY, 0.0f, DIRECTIONAL_LIGHT);
  }

  glm::vec3 ambient() {
    return glm::vec3(0.2);
  }

  glm::vec3 diffuse() {
    return glm::vec3(brightness);
  }

  glm::vec3 specular() {
    return glm::vec3(brightness);
  }
};

struct Light {
  // When the w component is 0,
  // "position" is actually direction.
  glm::vec4 position;
  glm::vec3 color;

  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;

  // Attenuation
  float constant;
  float linear;
  float quadratic;
};

class Renderer {
  private:
    float xAxisVertices[6] {
      -512.0f, 0.0f, 0.0f,
      512.0f, 0.0f, 0.0f,
    };
    unsigned int xAxisVAO {};
    unsigned int xAxisVBO {};

    float yAxisVertices[6] {
      0.0f, -512.0f, 0.0f,
      0.0f, 512.0f, 0.0f
    };
    unsigned int yAxisVAO {};
    unsigned int yAxisVBO {};

    float zAxisVertices[6] {
      0.0f, 0.0f, -512.0f,
      0.0f, 0.0f, 512.0f,
    };
    unsigned int zAxisVAO {};
    unsigned int zAxisVBO {};

    Shader blockShader { Shader("./src/shaders/block.vert", "./src/shaders/block.frag") };
    Shader lineShader { Shader("./src/shaders/line.vert", "./src/shaders/line.frag") };
    Shader normalShader { Shader("./src/shaders/normal.vert", "./src/shaders/normal.frag", "./src/shaders/normal.geom") };

    bool wireMode { false };

    Camera camera { Camera() };
    glm::ivec3 lastCameraChunkPosition {};

    std::unordered_map<xyz, Chunk, hash_tuple::hash<xyz>> chunks {};
    int viewingDistance { 6 };
    std::unordered_set<xyz, hash_tuple::hash<xyz>> lastAreaOfInterest {};

    // Lighting
    SunMoon sunMoon {
      160,             // angleInDegrees
      glm::vec3(1.0f), // color
      0.7f,            // brightness - 0.2f (give or take) for Night, 0.7f (give or take) for Day.
    };
    float torchConstant = 1.0f;
    float torchLinear = 0.1f;
    float torchQuadratic = 0.1f;

    Light redLight {
      glm::vec4(12.0f, 96.0f, -1.0f, POINT_LIGHT),
      glm::vec3(1.0f, 0.0f, 0.0f),
      glm::vec3(1.0f),
      glm::vec3(0.0f),
      glm::vec3(0.0f),
      torchConstant,
      torchLinear,
      torchQuadratic,
    };

    Light greenLight {
      glm::vec4(12.0f, 96.0f, 1.0f, POINT_LIGHT),
      glm::vec3(0.0f, 1.0f, 0.0f),
      glm::vec3(1.0f),
      glm::vec3(0.0f),
      glm::vec3(0.0f),
      torchConstant,
      torchLinear,
      torchQuadratic,
    };

    Light blueLight {
      glm::vec4(10.0f, 96.0f, 0.0f, POINT_LIGHT),
      glm::vec3(0.0f, 0.0f, 1.0f),
      glm::vec3(1.0f),
      glm::vec3(0.0f),
      glm::vec3(0.0f),
      torchConstant,
      torchLinear,
      torchQuadratic,
    };

    Light lights[3] { redLight, greenLight, blueLight };

    // Shadows
    const int SHADOW_WIDTH { 2048 };
    const int SHADOW_HEIGHT { 2048 };
    unsigned int shadowMapFBO {};
    unsigned int shadowMap {};
    Shader simpleDepthShader { Shader("./src/shaders/simple_depth_shader.vert", "./src/shaders/simple_depth_shader.frag") };
    Shader debugDepthShader { Shader("./src/shaders/debug_depth.vert", "./src/shaders/debug_depth.frag") };
    bool showDepthMap { true };
    bool debugShadows { true };
    void renderDepthmapDebug();
    float orthoLeft { -200.0f };
    float orthoRight { 200.0f };
    float orthoBottom { -200.0f };
    float orthoTop { 200.0f };
    float orthoNear { camera.nearPlane };
    float orthoFar { camera.farPlane };
    float shadowAcneBias { 0.0007 };

    // FPS
    // https://stackoverflow.com/a/4687507
    int framesThisSecond { -1 };
    float currentFPS { 0 };
    float avgFPS { 60.0f };
    float fpsDecay { 0.9f };
    double lastSecond { glfwGetTime() };
    void calculateFPS();

    // Normals
    bool showNormals { false };
    void renderNormals();

    // Coordinate lines
    bool showCoordinateLines { true };
    void renderCoordinateLines();

    // Overlay
    bool showOverlay { true };
    void renderOverlay();

    // ...
    void renderScene(const Shader& shader);
  public:
    Renderer();
    void update(double dt);
    void render(double dt);
    void processInput(GLFWwindow* window, float dt);
    void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void windowSizeCallback(GLFWwindow* window, int width, int height);
};
