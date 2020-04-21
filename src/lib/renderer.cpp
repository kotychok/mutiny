#include <algorithm>
#include <array>
#include <experimental/filesystem>
#include <iostream>
#include <thread>

#include <imgui/imgui.h>
#include <stb/stb_image.h>
#include <glm/gtx/vector_angle.hpp>

#include "memory_helper.h"
#include "mesher_greedy.h"
#include "renderer.h"
#include "shader.h"
#include "texture.h"
#include "texture_unit.h"
#include "print_tuple.h"
#include "print_vector.h"

// Debugging
#include <glm/gtx/string_cast.hpp>

Renderer::Renderer() {
  std::cout << "Renderer created" << std::endl;

  stbi_set_flip_vertically_on_load(true);

  glEnable(GL_DEPTH_TEST);

  //
  // Block textures
  //
  blocksTextureUnitIndex = TextureUnit::reserveTextureUnit();
  TextureUnit::activate(blocksTextureUnitIndex);
  // TODO Thinking of moving to a service oriented design. LoadsBlockTextures::call();
  Texture::loadBlockTextures();

  //
  // Depthmap texture
  //
  depthMapTextureUnitIndex = TextureUnit::reserveTextureUnit();
  TextureUnit::activate(depthMapTextureUnitIndex);
  glGenFramebuffers(1, &depthMapFBO);
  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
      SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  // Attach depth texture as FBO's depth buffer.
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //
  // Astronomical bodies textures
  //
  astronomicalBodiesTextureUnitIndex = TextureUnit::reserveTextureUnit();
  TextureUnit::activate(astronomicalBodiesTextureUnitIndex);
  // TODO Thinking of moving to a service oriented design. LoadsAstronomicalBodiesTextures::call();
  Texture::loadAstronomicalBodiesTextures();
}

void Renderer::processInput(GLFWwindow* window, float dt) {
  camera.processInput(window, dt);
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
          Chunk &chunk = chunks.try_emplace(key, glm::vec3(ix, iy, iz), ChunkGenerator::perlin).first->second;

          // This is implicitly set when the object is created, but I want to
          // be explicit here about the fact that the chunk cannot be deleted
          // if there is going to be a thread that needs it.
          //
          // Since the setMesh method sets canBeUnloaded to true, after the
          // thread is done, we'll be able to delete it.
          chunk.canBeUnloaded = false;

          // Then generate its mesh
          threadPool.push(
            [](int i, Chunk& chunk) {
              // std::cout << "Meshing: " << chunk.pos << std::endl;
              std::vector<float> mesh = MesherGreedy::computeChunkMesh(chunk);
              chunk.setMesh(mesh);
            },
            std::ref(chunk)
          );
        }
      }
    }
  }

  // FIXME This detection doesn't work super well anymore.
  // Since the chunk might not be ready to be unloaded yet, it's possible to
  // have orphaned chunks remain around forever (or until you go back up to
  // them and they get unloaded the time time)
  //
  // I wonder if there is a mathy way to be able to do this. I should be able
  // to compute the list of keys to delete based off of viewing distance and then
  // erase all those chunks.
  //
  // FIXME 2 Also, I might be able to change or remove the other bookkeeping
  // methods like lastAreaOfInterest idk.
  //
  // ---
  //
  // Actually, to fix this, instead of using lastAreaOfInterest, can I get rid of it entirely and just use chunk.keys?
  // ---
  //
  // If the current AoI doesn't have a previous AoI key, then it is outside
  // of the AoI and we want to unload it.
  for (const auto& c : chunks) {
    xyz chunkKey = c.first;
    if (areaOfInterest.find(chunkKey) == areaOfInterest.end()) {
      chunksToBeUnloaded.insert(chunkKey);
    }
  }

  for (xyz chunkKey : chunksToBeUnloaded) {
    Chunk& chunk = chunks[chunkKey];
    if (chunk.canBeUnloaded) {
      chunks.erase(chunkKey);
    }
  }

  chunksToBeUnloaded.clear();

  // Get ready for the next frame
  lastCameraChunkPosition = cameraChunkPosition;
  lastAreaOfInterest = areaOfInterest;
}

void Renderer::render(double dt) {
  calculateFPS();

  renderSceneToDepthMap();

  renderSceneToScreen();

  renderAstronomicalBodies();

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
    renderDepthMap();
  }
}
#pragma GCC diagnostic pop

void Renderer::renderSceneToDepthMap() {
  float sceneSize = (2 * viewingDistance + 1) * CHUNK_SIZE;

  // The light frustrum is at its largest size when the angle between the light
  // and the x-axis is 45 or 135 degrees.
  //
  // We calculate this largest size so that at any angle the entire scene is
  // captured to generate shadows.
  float orthoRight { sceneSize / 2 };
  float orthoLeft { -1 * orthoRight };
  float orthoTop { sceneSize / static_cast<float>(sqrt(2)) };
  float orthoBottom { -1 * orthoTop };
  float orthoNear { 0.001f };
  float orthoFar { sceneSize * static_cast<float>(sqrt(2)) };
  glm::mat4 lightProjection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, orthoNear, orthoFar);

  glm::vec3 direction {};
  // Switch which light source causes shadows at dawn/dusk.
  // This is the best way to transition that I've found so that shadows don't
  // suddenly appear out of nowhere.
  if (sun.angle(timeOfDay) <= 90) {
    direction = sun.direction(timeOfDay);
  } else {
    direction = moon.direction(timeOfDay);
  }

  // The light position needs to be far enough way to contain the entire scene.
  // I add + 1 at the end to make sure the camera is _just_ outside the
  // outermost chunk. It may be unnecessary.
  glm::vec3 lightPosition = ((viewingDistance + 1) * CHUNK_SIZE + 1) * glm::vec3(-direction);

  // We need to translate the point we are looking at along x and z so that
  // we are looking at the center x and center z of the loaded chunks.
  glm::vec3 sceneCenter = glm::vec3(lastCameraChunkPosition.x * CHUNK_SIZE, 0.0f, lastCameraChunkPosition.z * CHUNK_SIZE);

  glm::mat4 lightView = glm::lookAt(
    // The light frustrum needs to move with the scene.
    lightPosition + sceneCenter,

    // The center of the scene, i.e. the light target
    sceneCenter,

    glm::vec3(0.0f, 1.0f, 0.0f)
  );
  lightSpaceMatrix = lightProjection * lightView;

  simpleDepthShader.use();
  simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

  glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glClear(GL_DEPTH_BUFFER_BIT);
  renderScene(simpleDepthShader);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::renderSceneToScreen() {
  glViewport(0, 0, Window::WIDTH, Window::HEIGHT);
  glm::vec3 currentSkyColor = skyColor();
  glClearColor(currentSkyColor.r, currentSkyColor.g, currentSkyColor.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (wireMode) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  blockShader.use();

  // Vertex Shader Uniforms
  blockShader.setMat4("view", camera.getViewMatrix());
  blockShader.setMat4("projection", camera.getProjectionMatrix());
  blockShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

  // Fragment Block Uniforms
  blockShader.setInt("blockTexturesArray", blocksTextureUnitIndex);

  // Fragment Lighting Uniforms
  blockShader.setVec3("cameraPosition", camera.position);
  blockShader.setFloat("material.shininess", 32.0f);

  blockShader.setVec4("lights[0].position", sun.direction(timeOfDay));
  blockShader.setVec3("lights[0].color", sun.color);
  blockShader.setVec3("lights[0].ambient", sun.ambient(timeOfDay));
  blockShader.setVec3("lights[0].diffuse", sun.diffuse(timeOfDay));
  blockShader.setVec3("lights[0].specular", sun.specular(timeOfDay));
  blockShader.setFloat("lights[0].constant", 0.0f);
  blockShader.setFloat("lights[0].linear", 0.0f);
  blockShader.setFloat("lights[0].quadratic", 0.0f);

  blockShader.setVec4("lights[4].position", moon.direction(timeOfDay));
  blockShader.setVec3("lights[4].color", moon.color);
  blockShader.setVec3("lights[4].ambient", moon.ambient(timeOfDay));
  blockShader.setVec3("lights[4].diffuse", moon.diffuse(timeOfDay));
  blockShader.setVec3("lights[4].specular", moon.specular(timeOfDay));
  blockShader.setFloat("lights[4].constant", 0.0f);
  blockShader.setFloat("lights[4].linear", 0.0f);
  blockShader.setFloat("lights[4].quadratic", 0.0f);

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

  // Fragment Shadow Uniforms
  blockShader.setBool("enableShadows", enableShadows);
  blockShader.setInt("depthMap", depthMapTextureUnitIndex);
  blockShader.setFloat("shadowMultiplier", shadowMultiplier);
  blockShader.setFloat("shadowAcneBias", shadowAcneBias);
  blockShader.setBool("debugShadows", debugShadows);

  // Fragment Other Uniforms
  blockShader.setFloat("ellapsedTime", glfwGetTime());

  renderScene(blockShader);
}

void Renderer::renderAstronomicalBodies() {
  // We only need a single vao for the vertices since they're shared between
  // sun and moon.
  static unsigned int vao {};
  static unsigned int vbo {};
  static unsigned int ebo {};

  if (vao == 0) {
    float vertices[] = {
      // positions        // colors         // texture coords
       0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
       0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
      -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    };
    int indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3, // second triangle
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  }

  astronomicalBodiesShader.use();
  astronomicalBodiesShader.setInt("astronomicalBodiesTexturesArray", astronomicalBodiesTextureUnitIndex);
  astronomicalBodiesShader.setMat4("view", camera.getViewMatrix());
  astronomicalBodiesShader.setMat4("projection", camera.getProjectionMatrix());

  std::array<DirectionalLight*, 2> bodies { &sun, &moon };
  for (unsigned int textureIndex = 0; textureIndex < bodies.size(); ++textureIndex) {
    astronomicalBodiesShader.setFloat("textureIndex", static_cast<float>(textureIndex));

    DirectionalLight* bodyPtr = bodies.at(textureIndex);

    glm::vec4 direction = bodyPtr->direction(timeOfDay);
    glm::mat4 model = glm::mat4(1);

    // Astronomical bodies should stay put w/r/t the camera to give the
    // appearance that they are very far away.
    model = glm::translate(model, camera.position);

    // Put it in the sky instead of at the origin.
    model = glm::translate(model, -100.0 * glm::vec3(direction));

    // Angle the texture so that when it's translated by the direction, it's
    // facing the origin.
    model = glm::rotate(model, glm::angle(glm::vec3(direction), glm::vec3(0.0f, 1.0f, 0.0f)), glm::vec3(0.0f, 0.0f, 1.0f));

    // Make texture face the y-axis.
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    astronomicalBodiesShader.setMat4("model", model);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }

  glBindVertexArray(0);
}

void Renderer::renderScene(const Shader& shader) {
  for (std::pair<const xyz, Chunk>& kv : chunks) {
    Chunk& chunk = kv.second;
    chunk.render(shader);
  }
}

void Renderer::renderDepthMap() {
  const int depthMapSize = 300;
  glViewport(Window::WIDTH - depthMapSize, Window::HEIGHT - depthMapSize, depthMapSize, depthMapSize);

  debugDepthShader.use();
  debugDepthShader.setInt("depthMap", 1);

  // These aren't actually used since I use the orthographic view, and they are
  // only used for the perspective view.
  // I'm really not sure what the perspective view in this shader would be for.
  debugDepthShader.setFloat("nearPlane", camera.nearPlane);
  debugDepthShader.setFloat("farPlane", camera.farPlane);

  static unsigned int depthVAO { 0 };
  static unsigned int depthVBO;
  if (depthVAO == 0)
  {
      float vertices[] = {
          // positions        // texture Coords
          -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
          -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
           1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
           1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
      };

      // setup plane VAO
      glGenVertexArrays(1, &depthVAO);
      glGenBuffers(1, &depthVBO);

      glBindVertexArray(depthVAO);
      glBindBuffer(GL_ARRAY_BUFFER, depthVBO);

      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  }

  glBindVertexArray(depthVAO);
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
  lineShader.use();

  static unsigned int xAxisVAO {};
  static unsigned int xAxisVBO {};
  static unsigned int yAxisVAO {};
  static unsigned int yAxisVBO {};
  static unsigned int zAxisVAO {};
  static unsigned int zAxisVBO {};

  // We only need to check one
  if (xAxisVAO == 0) {
    lineShader.setMat4("model", glm::mat4(1.0f));

    float xAxisVertices[6] {
      -512.0f, 0.0f, 0.0f,
      512.0f, 0.0f, 0.0f,
    };

    float yAxisVertices[6] {
      0.0f, -512.0f, 0.0f,
      0.0f, 512.0f, 0.0f
    };

    float zAxisVertices[6] {
      0.0f, 0.0f, -512.0f,
      0.0f, 0.0f, 512.0f,
    };

    glGenVertexArrays(1, &xAxisVAO);
    glGenBuffers(1, &xAxisVBO);

    glGenVertexArrays(1, &yAxisVAO);
    glGenBuffers(1, &yAxisVBO);

    glGenVertexArrays(1, &zAxisVAO);
    glGenBuffers(1, &zAxisVBO);

    glBindVertexArray(xAxisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, xAxisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(xAxisVertices), xAxisVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(yAxisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, yAxisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(yAxisVertices), yAxisVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(zAxisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, zAxisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(zAxisVertices), zAxisVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
  }

  lineShader.setMat4("view", camera.getViewMatrix());
  lineShader.setMat4("projection", camera.getProjectionMatrix());

  glBindVertexArray(xAxisVAO);
  lineShader.setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));
  glDrawArrays(GL_LINES, 0, 2);

  glBindVertexArray(yAxisVAO);
  lineShader.setVec3("color", glm::vec3(0.0f, 1.0f, 0.0f));
  glDrawArrays(GL_LINES, 0, 2);

  glBindVertexArray(zAxisVAO);
  lineShader.setVec3("color", glm::vec3(0.0f, 0.0f, 1.0f));
  glDrawArrays(GL_LINES, 0, 2);

  glBindVertexArray(0);
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

    ImGui::Text("Time");
    int hours = static_cast<int>(timeOfDay);
    int minutes = static_cast<int>(timeOfDay * 60) % 60;
    std::string timeDisplay = std::to_string(hours) + "h " + std::to_string(minutes) + "m";
    ImGui::SliderFloat("Time of Day", &timeOfDay, 0.0f, 24.0f, timeDisplay.c_str());

    if (ImGui::Button("Midnight")) {
      timeOfDay = 0.0;
    }
    ImGui::SameLine();
    if (ImGui::Button("Dawn")) {
      timeOfDay = 6.0;
    }
    ImGui::SameLine();
    if (ImGui::Button("Midday")) {
      timeOfDay = 12.0;
    }
    ImGui::SameLine();
    if (ImGui::Button("Dusk")) {
      timeOfDay = 18.0;
    }
    ImGui::SameLine();
    if (ImGui::Button("Midnight")) {
      timeOfDay = 24.0;
    }

    ImGui::Separator();

    ImGui::Text("Sun");
    ImGui::Text("Sun Angle %.0f", sun.angle(timeOfDay));
    ImGui::Text("Sun Direction: (%.2f,%.2f, %.2f)", sun.direction(timeOfDay).x, sun.direction(timeOfDay).y, sun.direction(timeOfDay).z);

    ImGui::Checkbox("Use Sun Overrides?", &sun.useOverrides);
    ImGui::SliderFloat("Sun Ambient Override", &sun.ambientStrengthOverride, 0.0f, 1.0f);
    ImGui::SliderFloat("Sun Diffuse Override", &sun.diffuseStrengthOverride, 0.0f, 1.0f);
    ImGui::SliderFloat("Sun Specular Override", &sun.specularStrengthOverride, 0.0f, 1.0f);

    ImGui::Text("Ambient Str %.2f", sun.ambientStrength(timeOfDay));
    ImGui::Text("Diffuse Str %.2f", sun.diffuseStrength(timeOfDay));
    // TODO For consistency I should make this like the others even if it's 0.
    ImGui::Text("Specular Str %.2f", glm::length(sun.specular(timeOfDay)));

    ImGui::Separator();

    ImGui::Text("Shadows");
    ImGui::Checkbox("Enable Shadows?", &enableShadows);
    ImGui::Checkbox("Show Depth Map Debug?", &showDepthMap);
    ImGui::Checkbox("Debug Shadows?", &debugShadows);
    ImGui::SliderFloat("Shadow Multiplier", &shadowMultiplier, 0.0f, 1.0f);
    ImGui::InputFloat("Shadow Acne Bias", &shadowAcneBias, 0.00001f, 0.00001f, "%.5f");

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

glm::vec3 Renderer::skyColor() {
  float interpolation = abs((timeOfDay - 12) / 12);
  glm::vec3 middayColor = glm::vec3(0.53f, 0.81f, 0.92f);
  glm::vec3 midnightColor = glm::vec3(0.1f, 0.1f, 0.12f);
  return middayColor + (midnightColor - middayColor) * interpolation;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void Renderer::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
  camera.cursorPosCallback(window, xpos, ypos);
}

void Renderer::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_F3:
        showOverlay = !showOverlay;
        break;
    }
  }
}

void Renderer::windowSizeCallback(GLFWwindow* window, int width, int height) {
  camera.windowSizeCallback(window, width, height);
}
#pragma GCC diagnostic pop
