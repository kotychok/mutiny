#include <algorithm>
#include <experimental/filesystem>
#include <iostream>

#include <imgui/imgui.h>
#include <stb/stb_image.h>

#include "memory_helper.h"
#include "mesher_greedy.h"
#include "renderer.h"
#include "shader.h"
#include "texture.h"

Renderer::Renderer() {
  std::cout << "Renderer created" << std::endl;

  stbi_set_flip_vertically_on_load(true);

  glEnable(GL_DEPTH_TEST);

  glGenVertexArrays(1, &xAxisVAO);
  glGenBuffers(1, &xAxisVBO);

  glGenVertexArrays(1, &yAxisVAO);
  glGenBuffers(1, &yAxisVBO);

  glGenVertexArrays(1, &zAxisVAO);
  glGenBuffers(1, &zAxisVBO);

  glActiveTexture(GL_TEXTURE0);
  Texture::loadBlockTextures();

  glActiveTexture(GL_TEXTURE1);
  glGenFramebuffers(1, &shadowMapFBO);
  glGenTextures(1, &shadowMap);
  glBindTexture(GL_TEXTURE_2D, shadowMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
      SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // Attach depth texture as FBO's depth buffer.
  // TODO replace with direct state call instead?
  glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  debugDepthShader.use();
  debugDepthShader.setInt("depthMap", 1);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void Renderer::update(double dt) {
  glm::ivec3 cameraChunkPosition {};

  if (camera.position.x > 0) {
    cameraChunkPosition.x = floor((camera.position.x + CHUNK_SIZE_HALVED) / CHUNK_SIZE);
  } else {
    cameraChunkPosition.x = ceil((camera.position.x - CHUNK_SIZE_HALVED) / CHUNK_SIZE);
  }

  if (camera.position.z > 0) {
    cameraChunkPosition.z = floor((camera.position.z + CHUNK_SIZE_HALVED) / CHUNK_SIZE);
  } else {
    cameraChunkPosition.z = ceil((camera.position.z - CHUNK_SIZE_HALVED) / CHUNK_SIZE);
  }

  cameraChunkPosition.y = floor((camera.position.y) / CHUNK_SIZE);

  std::unordered_set<xyz, hash_tuple::hash<xyz>> areaOfInterest {};

  for (auto ix = cameraChunkPosition.x - viewingDistance; ix <= cameraChunkPosition.x + viewingDistance; ix++) {
    for (auto iz = cameraChunkPosition.z - viewingDistance; iz <= cameraChunkPosition.z + viewingDistance; iz++) {
      for (auto iy = std::max(0, cameraChunkPosition.y - viewingDistance); iy <= cameraChunkPosition.y + viewingDistance; iy++) {
        xyz key = std::make_tuple(ix, iy, iz);

        areaOfInterest.insert(key);

        if (chunks.find(key) == chunks.end()) {
          // If our chunk is not loaded, we need to create it
          Chunk &chunk = chunks.try_emplace(key, glm::vec3(ix, iy, iz), ChunkGenerator::flatWithPlus).first->second;

          // Then generate its mesh
          std::vector<float> mesh = MesherGreedy::computeChunkMesh(chunk);
          chunk.setMesh(mesh);
        }
      }
    }
  }

  if (cameraChunkPosition != lastCameraChunkPosition) {
    std::cout << "\nChunk changed!!" << std::endl;

    // If the current AoI doesn't have a previous AoI key, then it is outside
    // of the AoI and we want to unload it.
    for (xyz oldKey : lastAreaOfInterest) {
      if (areaOfInterest.find(oldKey) == areaOfInterest.end()) {
        chunks.erase(oldKey);
      }
    }
  }

  // Get ready for the next frame
  lastCameraChunkPosition = cameraChunkPosition;
  lastAreaOfInterest = areaOfInterest;
}

void Renderer::render(double dt) {
  calculateFPS();

  glClearColor(0.53f, 0.81f, 0.92f, 1.0f); // Day
  // glClearColor(0.1f, 0.1f, 0.12f, 1.0f); // Night
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //
  // ******* Render Shadow Map *******
  //
  // TODO The ortho values probs need to be set based on the loaded chunks. I think these
  // are world position coordinates. Something like cameraPos +- visibilityDistance * chunk_size
  // float frustrumDistance = (0.5f + visibilityDistance
  // glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 150.0f, camera.nearPlane, camera.farPlane);
  glm::mat4 lightProjection = glm::ortho(-17.0f, 17.0f, -17.0f, 17.0f, camera.nearPlane, camera.farPlane);
  glm::mat4 lightView = glm::lookAt(
    glm::vec3(-sunMoon.position()),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
  );
  glm::mat4 lightSpaceMatrix = lightProjection * lightView;

  simpleDepthShader.use();
  simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

  glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
  glClear(GL_DEPTH_BUFFER_BIT);
  renderScene(simpleDepthShader);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //
  // ******* Render Regular Scene *******
  //
  glViewport(0, 0, Window::WIDTH, Window::HEIGHT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (wireMode) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  blockShader.use();

  blockShader.setMat4("view", camera.getViewMatrix());
  blockShader.setMat4("projection", camera.getProjectionMatrix());
  blockShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

  blockShader.setInt("myTexture", 0);
  blockShader.setFloat("ellapsedTime", glfwGetTime());
  blockShader.setVec3("cameraPosition", camera.position);
  blockShader.setInt("shadowMap", 1);
  blockShader.setInt("debugShadows", debugShadows);

  blockShader.setVec4("lights[0].position", sunMoon.position());
  blockShader.setVec3("lights[0].color", sunMoon.color);
  blockShader.setVec3("lights[0].ambient", sunMoon.ambient());
  blockShader.setVec3("lights[0].diffuse", sunMoon.diffuse());
  blockShader.setVec3("lights[0].specular", sunMoon.specular());
  blockShader.setFloat("lights[0].constant", 0.0f);
  blockShader.setFloat("lights[0].linear", 0.0f);
  blockShader.setFloat("lights[0].quadratic", 0.0f);

  for (unsigned int i = 1; i < 4; ++i) {
    Light& light = lights[i-1];
    blockShader.setVec4("lights[" + std::to_string(i) + "].position", light.position);
    blockShader.setVec3("lights[" + std::to_string(i) + "].color", light.color);
    blockShader.setVec3("lights[" + std::to_string(i) + "].ambient", light.ambient);
    blockShader.setVec3("lights[" + std::to_string(i) + "].diffuse", light.diffuse);
    blockShader.setVec3("lights[" + std::to_string(i) + "].specular", light.specular);
    blockShader.setFloat("lights[" + std::to_string(i) + "].constant", light.constant);
    blockShader.setFloat("lights[" + std::to_string(i) + "].linear", light.linear);
    blockShader.setFloat("lights[" + std::to_string(i) + "].quadratic", light.quadratic);
  }

  blockShader.setFloat("material.shininess", 32.0f);

  renderScene(blockShader);

  if (showNormals) {
    renderNormals();
  }

  if (showCoordinateLines) {
    renderCoordinateLines();
  }

  // Do this at the end so that we have the most up-to-date info for this frame.
  if (showOverlay) {
    renderOverlay();
  }

  // Other overlays happen last so as to not mess up glViewport for others
  if (showDepthMap) {
    glViewport(0, 0, 300, 300);
    renderDepthmapDebug();
  }
}

void Renderer::renderScene(const Shader& shader) {
  for (std::pair<const xyz, Chunk>& kv : chunks) {
    Chunk& chunk = kv.second;
    chunk.render(shader);
  }
}
#pragma GCC diagnostic pop

void Renderer::processInput(GLFWwindow* window, float dt) {
  camera.processInput(window, dt);
}

void Renderer::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
  camera.cursorPosCallback(window, xpos, ypos);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void Renderer::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_F3:
        showOverlay = !showOverlay;
        break;
    }
  }
}
#pragma GCC diagnostic pop

void Renderer::windowSizeCallback(GLFWwindow* window, int width, int height) {
  camera.windowSizeCallback(window, width, height);
}

void Renderer::calculateFPS() {
  double now = glfwGetTime();
  double diff = now - lastSecond;
  if (diff > 1) {
    lastSecond = now;
    avgFPS = fpsDecay * avgFPS + (1.0 - fpsDecay) * framesThisSecond;
    framesThisSecond = -1;
  }
  framesThisSecond++;
  if (diff > 0) {
    currentFPS = framesThisSecond / diff;
  }
}

void Renderer::renderDepthmapDebug() {
  debugDepthShader.use();
  debugDepthShader.setFloat("near_plane", camera.nearPlane);
  debugDepthShader.setFloat("far_plane", camera.farPlane);

  static unsigned int quadVAO = 0;
  static unsigned int quadVBO;
  if (quadVAO == 0)
  {
      float quadVertices[] = {
          // positions        // texture Coords
          -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
          -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
           1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
           1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
      };
      // setup plane VAO
      glGenVertexArrays(1, &quadVAO);
      glGenBuffers(1, &quadVBO);
      glBindVertexArray(quadVAO);
      glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  }
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void Renderer::renderNormals() {
  normalShader.use();
  normalShader.setMat4("view", camera.getViewMatrix());
  normalShader.setMat4("projection", camera.getProjectionMatrix());
  renderScene(normalShader);
}

void Renderer::renderCoordinateLines() {
  glm::mat4 axisModel = glm::mat4(1.0f);
  lineShader.use();
  lineShader.setMat4("view", camera.getViewMatrix());
  lineShader.setMat4("projection", camera.getProjectionMatrix());
  lineShader.setMat4("model", axisModel);

  glBindVertexArray(xAxisVAO);
  glBindBuffer(GL_ARRAY_BUFFER, xAxisVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(xAxisVertices), xAxisVertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  lineShader.setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));
  glDrawArrays(GL_LINES, 0, 2);
  glDrawArrays(GL_POINTS, 0, 2);

  glBindVertexArray(yAxisVAO);
  glBindBuffer(GL_ARRAY_BUFFER, yAxisVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(yAxisVertices), yAxisVertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  lineShader.setVec3("color", glm::vec3(0.0f, 1.0f, 0.0f));
  glDrawArrays(GL_LINES, 0, 2);
  glDrawArrays(GL_POINTS, 0, 2);

  glBindVertexArray(zAxisVAO);
  glBindBuffer(GL_ARRAY_BUFFER, zAxisVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(zAxisVertices), zAxisVertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  lineShader.setVec3("color", glm::vec3(0.0f, 0.0f, 1.0f));
  glDrawArrays(GL_LINES, 0, 2);
  glDrawArrays(GL_POINTS, 0, 2);
}

void Renderer::renderOverlay() {
  ImGuiIO& io = ImGui::GetIO();

  const float DISTANCE = 10.0f;
  ImVec2 window_pos = ImVec2(DISTANCE, DISTANCE);
  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background

  const auto flags =
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoDecoration |
    ImGuiWindowFlags_AlwaysAutoResize |
    ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_NoFocusOnAppearing |
    ImGuiWindowFlags_NoNav;

  if (ImGui::Begin("Info", &showOverlay, flags)) {
    ImGui::Text("Debug Info");

    ImGui::Separator();

    ImGui::Text("Mouse:");
    if (ImGui::IsMousePosValid()) {
      ImGui::Text("(%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
    } else {
      ImGui::Text("<invalid>");
    }

    ImGui::Separator();

    ImGui::Text("Camera:");
    ImGui::Text("Position: (%.0f,%.0f, %.0f)", camera.position.x, camera.position.y, camera.position.z);
    ImGui::Text("Chunk: (%i,%i, %i)", lastCameraChunkPosition.x, lastCameraChunkPosition.y, lastCameraChunkPosition.z);
    ImGui::SliderFloat("FOV", &camera.fov, Camera::MIN_FOV, Camera::MAX_FOV);
    ImGui::SliderFloat("Near Plane", &camera.nearPlane, Camera::MIN_NEAR_PLANE, Camera::MAX_NEAR_PLANE);
    ImGui::SliderFloat("Far Plane", &camera.farPlane, Camera::MIN_FAR_PLANE, Camera::MAX_FAR_PLANE);
    ImGui::SliderFloat("Camera Speed", &camera.speed, Camera::MIN_SPEED, Camera::MAX_SPEED);
    ImGui::SliderFloat("Mouse Sensitivity", &camera.sensitivity, Camera::MIN_SENSITIVITY, Camera::MAX_SENSITIVITY);

    ImGui::Separator();

    ImGui::Text("Rendering:");
    ImGui::Checkbox("Wire mode?", &wireMode);
    ImGui::Checkbox("Show Normals?", &showNormals);
    ImGui::Checkbox("Show Coordinate Lines?", &showCoordinateLines);
    ImGui::SliderInt("Viewing Distance", &viewingDistance, 0, 10);
    int viewingDistanceDiameter { 2 * viewingDistance + 1 };
    ImGui::Text(
      "Defined Area of Interest: %ix%ix%i (%i chunks)",
      viewingDistanceDiameter, viewingDistanceDiameter, viewingDistanceDiameter,
      viewingDistanceDiameter * viewingDistanceDiameter * viewingDistanceDiameter
    );
    ImGui::Text("Chunks loaded: %lu", chunks.size());
    ImGui::Text("lastAreaOfInterest size: %lu", lastAreaOfInterest.size());

    ImGui::Separator();

    ImGui::Text("Lighting");
    ImGui::SliderFloat("Sun & Moon Angle", &sunMoon.angleInDegrees, 180.0f, 0.0f);
    ImGui::Text("Sun & Moon Direction: (%.2f,%.2f, %.2f)", sunMoon.position().x, sunMoon.position().y, sunMoon.position().z);
    ImGui::Text("Sun & Moon Position: (%.2f,%.2f, %.2f)", (-sunMoon.position()).x, (-sunMoon.position()).y, (-sunMoon.position()).z);
    ImGui::SliderFloat("Sun & Moon Brightness", &sunMoon.brightness, 0.0f, 1.0f);
    ImGui::Checkbox("Show Depth Map Debug?", &showDepthMap);
    ImGui::Checkbox("Debug Shadows?", &debugShadows);

    ImGui::Separator();

    ImGui::Text("Profiling");
    double vm, rss;
    MemoryHelper::processMemUsage(vm, rss);
    ImGui::Text("VM: %.0f, RSS: %.0f", vm, rss);
    ImGui::Text("AVG FPS: %.0f", avgFPS);
    ImGui::Text("Current FPS: %.0f", currentFPS);
  }
  ImGui::End();
}
