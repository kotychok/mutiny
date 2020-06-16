#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/scalar_multiplication.hpp>

// Debugging
#include <glm/gtx/string_cast.hpp>

#include "constants.h"
#include "util/lerp.h"

class DirectionalLight {
  public:
    static const glm::vec4 baseOrigin;

    glm::vec3 color;
    float maxAmbientStrength;
    float illuminationEndAngle {};
    float ambientStrengthOverride;
    float diffuseStrengthOverride;
    float specularStrengthOverride;
    bool useOverrides;
    float offset;

    DirectionalLight(
      glm::vec3 color,
      float maxAmbientStrength,
      float illuminationEndAngle,
      float ambientStrengthOverride,
      float diffuseStrengthOverride,
      float specularStrengthOverride,
      bool useOverrides,
      float offset
    );

    // Explicitly delete since these shouldn't be used
    DirectionalLight(const DirectionalLight&) = delete; // Delete copy constructor
    DirectionalLight& operator=(const DirectionalLight&) = delete; // Delete copy assignment
    DirectionalLight(DirectionalLight&&) = delete; // Delete move constructor
    DirectionalLight& operator=(DirectionalLight &&) = delete; // Delete move assignment

    float angleToTime(float angle);
    float angle(float timeOfDay);

    // Returns a vector pointing from the light source to the scene.
    glm::vec4 direction(float timeOfDay);

    glm::vec4 origin();

    float ambientStrength(float angle);
    glm::vec3 ambient(float angle);

    float diffuseStrength(float angle);
    glm::vec3 diffuse(float angle);

    glm::vec3 specular(float angle);
  private:
};
