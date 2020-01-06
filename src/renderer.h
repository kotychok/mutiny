#pragma once

#include "map_with_tuple_keys.h"

#include <tuple>
#include <unordered_map>

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

class Renderer {
  private:
    float cubeVertices[180] {
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
       0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

      -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
       0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
       0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    unsigned int cubeVAO {};
    unsigned int cubeVBO {};

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
    Camera camera { Camera() };
    std::unordered_map<xyz, Chunk, hash_tuple::hash<xyz>> chunks {
      { std::make_tuple(0, 0, 0), Chunk(glm::vec3(0.0f, 0.0f, 0.0f)) },
      { std::make_tuple(-1,0,0), Chunk(glm::vec3(-1.0f, 0.0f, 0.0f)) },
      { std::make_tuple(-1,0,-1), Chunk(glm::vec3(-1.0f, 0.0f, -1.0f)) },
      { std::make_tuple(0,0,-1), Chunk(glm::vec3(0.0f, 0.0f, -1.0f)) },
    };
  public:
    Renderer();
    void render(double dt);
    void processInput(GLFWwindow* window, float dt);
    void mouseCallback(GLFWwindow* window, double xpos, double ypos);
};
