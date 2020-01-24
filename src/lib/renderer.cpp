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

  // Axes
  glGenVertexArrays(1, &xAxisVAO);
  glGenBuffers(1, &xAxisVBO);

  glGenVertexArrays(1, &yAxisVAO);
  glGenBuffers(1, &yAxisVBO);

  glGenVertexArrays(1, &zAxisVAO);
  glGenBuffers(1, &zAxisVBO);

  glActiveTexture(GL_TEXTURE0);
  Texture::loadBlockTextures();
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void Renderer::render(double dt) {
  calculateFPS();

  glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (wireMode) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  blockShader.use();
  blockShader.setInt("myTexture", 0);
  blockShader.setMat4("view", camera.getViewMatrix());
  blockShader.setMat4("projection", camera.getProjectionMatrix());

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
          Chunk &chunk = chunks.try_emplace(key, glm::vec3(ix, iy, iz), ChunkGenerator::flatRandom).first->second;

          // Then generate its mesh
          // std::vector<std::pair<quad, BlockType>> quads = MesherGreedy::chunkToQuads(chunk);
          // std::vector<quad_mesh> quadMeshes {};
          // transform(quads.begin(), quads.end(), back_inserter(quadMeshes), MesherGreedy::quadToQuadMesh);

          std::vector<quad_mesh> quadMeshes = MesherGreedy::chunkToQuads(chunk);
          chunk.setMesh(quadMeshes);

          chunk.render(blockShader);
        } else {
          Chunk &chunk = chunks.find(key)->second;
          chunk.render(blockShader);
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

  lastCameraChunkPosition = cameraChunkPosition;
  lastAreaOfInterest = areaOfInterest;

  if (showCoordinateLines) {
    renderCoordinateLines();
  }

  // Do this at the end so that we have the most up-to-date info for this frame.
  if (showOverlay) {
    renderOverlay();
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

    ImGui::Text("Profiling");
    double vm, rss;
    MemoryHelper::processMemUsage(vm, rss);
    ImGui::Text("VM: %.0f, RSS: %.0f", vm, rss);
    ImGui::Text("AVG FPS: %.0f", avgFPS);
    ImGui::Text("Current FPS: %.0f", currentFPS);
  }
  ImGui::End();
}
