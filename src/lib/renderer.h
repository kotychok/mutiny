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
#include "directional_light.h"
#include "shader.h"

typedef std::tuple<int, int, int> xyz;

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
    int blocksTextureUnitIndex {};

    Shader blockShader { Shader("./src/shaders/block.vert", "./src/shaders/block.frag") };
    Shader lineShader { Shader("./src/shaders/line.vert", "./src/shaders/line.frag") };
    Shader normalShader { Shader("./src/shaders/normal.vert", "./src/shaders/normal.frag", "./src/shaders/normal.geom") };

    Camera camera { Camera() };
    glm::ivec3 lastCameraChunkPosition {};

    std::unordered_map<xyz, Chunk, hash_tuple::hash<xyz>> chunks {};
    int viewingDistance { 3 };
    std::unordered_set<xyz, hash_tuple::hash<xyz>> lastAreaOfInterest {};

    // *** Time ***
    float timeOfDay { 12 };
    glm::vec3 skyColor();

    // *** Lighting ***
    DirectionalLight sun {
        glm::vec3(1.0f, 1.0f, 1.0f), // color
        0.5f, // maxAmbientStrength
        108.0f, // illuminationEndAngle
        1.0f,  // ambientOverride
        1.0f,  // diffuseOverride
        1.0f,  // specularOverride
        false, // useOverrides
        0,     // offset
    };
    DirectionalLight moon {
      glm::vec3(1.0f, 1.0f, 1.0f), // color
      0.2f,
      108.0f,
      1.0f,  // ambientOverride
      1.0f,  // diffuseOverride
      1.0f,  // specularOverride
      false, // useOverrides
      180    // offset
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

    // *** Depth Map & Shadows ***
    bool enableShadows { true };
    int depthMapTextureUnitIndex {};
    unsigned int depthMapFBO {};
    unsigned int depthMap {};

    Shader simpleDepthShader { Shader("./src/shaders/simple_depth_shader.vert", "./src/shaders/simple_depth_shader.frag") };
    Shader debugDepthShader { Shader("./src/shaders/debug_depth.vert", "./src/shaders/debug_depth.frag") };

    // For a viewingDistance of 3. To have a larger viewingDistance or a
    // smaller shadow texture size, we'll need CSM.
    const int SHADOW_WIDTH { 16384 };
    const int SHADOW_HEIGHT { 16384 };

    glm::mat4 lightSpaceMatrix {};
    float shadowMultiplier { 0.4 };
    float shadowAcneBias { 0.0007 };
    bool debugShadows { false };

    // *** Astronomical Bodies ***
    int astronomicalBodiesTextureUnitIndex {};
    Shader astronomicalBodiesShader { Shader("./src/shaders/astronomical_bodies.vert", "./src/shaders/astronomical_bodies.frag") };

    // *** FPS ***
    // https://stackoverflow.com/a/4687507
    int framesThisSecond { -1 };
    float currentFPS { 0 };
    float avgFPS { 60.0f };
    float fpsDecay { 0.9f };
    double lastSecond { glfwGetTime() };
    void calculateFPS();

    // *** Rendering ***
    bool showNormals { false };
    void renderNormals();

    bool showCoordinateLines { true };
    void renderCoordinateLines();

    bool showOverlay { true };
    void renderOverlay();

    bool showDepthMap { true };
    void renderDepthMap();

    bool wireMode { false };

    void renderSceneToDepthMap();
    void renderSceneToScreen();
    void renderAstronomicalBodies();
    void renderScene(const Shader& shader);
  public:
    Renderer();

    void processInput(GLFWwindow* window, float dt);
    void update(double dt);
    void render(double dt);

    void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void windowSizeCallback(GLFWwindow* window, int width, int height);
};
