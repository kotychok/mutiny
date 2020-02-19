#include <iostream>
#include <cmath>

#include "directional_light.h"

float DirectionalLight::symmetricalAngleInDegrees(float timeOfDay) {
  return 180 * ((timeOfDay - 12.0f) / 12.0f);
}

glm::vec4 DirectionalLight::direction(float timeOfDay) {
  float angleInRadians { glm::radians(symmetricalAngleInDegrees(timeOfDay)) };
  double sunMoonX { sin(angleInRadians) };
  double sunMoonY { -cos(angleInRadians) };
  return glm::vec4(sunMoonX, sunMoonY, 0.0f, DIRECTIONAL_LIGHT);
}

DirectionalLight::DirectionalLight(
    glm::vec3 color,
    float maxAmbientStrength,
    float illuminationStartAngle,
    float illuminationEndAngle,
    float ambientStrengthOverride,
    float diffuseStrengthOverride,
    float specularStrengthOverride,
    bool useOverrides
    ) :
  color{color},
  maxAmbientStrength{maxAmbientStrength},
  illuminationStartAngle{illuminationStartAngle},
  illuminationEndAngle{illuminationEndAngle},
  ambientStrengthOverride{ambientStrengthOverride},
  diffuseStrengthOverride{diffuseStrengthOverride},
  specularStrengthOverride{specularStrengthOverride},
  useOverrides{useOverrides}
{}

float DirectionalLight::ambientStrength(float angle) {
  if (useOverrides) {
    return ambientStrengthOverride;
  }

  if (angle <= illuminationStartAngle) {
    return 0.0f;
  }
  if (angle >= illuminationEndAngle) {
    return 0.0f;
  }

  float interpolation = std::abs(angle / illuminationStartAngle);

  // FIXME Instead of linear, I think I want something more like a bell curve here
  float ambientStrength = maxAmbientStrength - lerp(0.0f, maxAmbientStrength, interpolation);
  return ambientStrength;
}

glm::vec3 DirectionalLight::ambient(float angle) {
  return glm::vec3(ambientStrength(angle));
}

float DirectionalLight::diffuseStrength(float angle) {
  if (useOverrides) {
    return diffuseStrengthOverride;
  }

  if (angle <= illuminationStartAngle) {
    return 0.0f;
  }
  if (angle >= illuminationEndAngle) {
    return 0.0f;
  }

  float interpolation = std::abs(angle / illuminationStartAngle);
  float diffuseStrength = maxAmbientStrength - lerp(0.0f, maxAmbientStrength, interpolation);
  return diffuseStrength;
}

glm::vec3 DirectionalLight::diffuse(float angle) {
  return glm::vec3(diffuseStrength(angle));
}

glm::vec3 DirectionalLight::specular(float angle) {
  if (useOverrides) {
    return glm::vec3(specularStrengthOverride);
  }
  // Disable specular for now
  return glm::vec3(0.0f * angle);
}
