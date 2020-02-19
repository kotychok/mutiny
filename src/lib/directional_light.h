#pragma once

#include <glm/glm.hpp>

#include "constants.h"
#include "lerp.h"

class DirectionalLight {
  public:
    // Returns the angle to the east/west from the up direction.
    //
    // @param timeOfDay [0, 24]
    // @return angle [-180, 180]
    //
    // name     | timeOfDay (deg) | symmetricalAngleInDegrees
    // -------------------------------------------------------
    // Midnight |  0h             | -180
    // Dawn     |  6h             | -90
    // Midday   | 12h             |  0
    // Dusk     | 18h             |  90
    // Midnight | 24h             |  180
    static float symmetricalAngleInDegrees(float timeOfDay);

    // Returns a vector pointing from the light source to the scene.
    static glm::vec4 direction(float timeOfDay);

    glm::vec3 color;
    float maxAmbientStrength;
    float illuminationStartAngle {};
    float illuminationEndAngle {};
    float ambientStrengthOverride;
    float diffuseStrengthOverride;
    float specularStrengthOverride;
    bool useOverrides;

    DirectionalLight(
      glm::vec3 color,
      float maxAmbientStrength,
      float illuminationStartAngle,
      float illuminationEndAngle,
      float ambientStrengthOverride,
      float diffuseStrengthOverride,
      float specularStrengthOverride,
      bool useOverrides
    );

    // Explicitly delete since these shouldn't be used
    DirectionalLight(const DirectionalLight&) = delete; // Delete copy constructor
    DirectionalLight& operator=(const DirectionalLight&) = delete; // Delete copy assignment
    DirectionalLight(DirectionalLight&&) = delete; // Delete move constructor
    DirectionalLight& operator=(DirectionalLight &&) = delete; // Delete move assignment

    float ambientStrength(float angle);
    glm::vec3 ambient(float angle);

    float diffuseStrength(float angle);
    glm::vec3 diffuse(float angle);

    glm::vec3 specular(float angle);
  private:
};
