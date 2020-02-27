#include <iostream>
#include <cmath>

#include <glm/gtx/vector_angle.hpp>

#include "directional_light.h"

// This is the vector that offset is respect to.
// I decided to have it at (0, 1, 0) which is up in the positive y-axis.
// This matches the "noon"/overhead position of e.g. the sun.
// It also makes the "pre-noon" and "post-noon" angles equal except for sign
// change.
const glm::vec4 DirectionalLight::baseOrigin { glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) };

DirectionalLight::DirectionalLight(
    glm::vec3 color,
    float maxAmbientStrength,
    float illuminationEndAngle,
    float ambientStrengthOverride,
    float diffuseStrengthOverride,
    float specularStrengthOverride,
    bool useOverrides,
    float offset
    ) :
  color{color},
  maxAmbientStrength{maxAmbientStrength},
  illuminationEndAngle{illuminationEndAngle},
  ambientStrengthOverride{ambientStrengthOverride},
  diffuseStrengthOverride{diffuseStrengthOverride},
  specularStrengthOverride{specularStrengthOverride},
  useOverrides{useOverrides},
  offset{offset}
{}

float DirectionalLight::angleToTime(float angle) {
  return fmod((angle + offset) * 12.0f / 180.0f + 12.0f, 24.0f);
}

float DirectionalLight::angle(float timeOfDay) {
  return glm::degrees(glm::angle(-glm::vec3(direction(timeOfDay)), glm::vec3(0.0f, 1.0f, 0.0f)));
}

glm::vec4 DirectionalLight::direction(float timeOfDay) {
  float normalized = (timeOfDay - 12.0f) / 12.0f; // Turns [0, 24] into [-12, 12] into [-1, 1]
  float rotationAngle = 180 * normalized; // Angle from origin is now [-180, 180]
  glm::vec4 direction = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f)) * origin();
  direction = -glm::normalize(direction);

  // Debugging
  // std::cout << "\n*** " << (offset == 0.0f ? "SUN" : "MOON") << " ***" << std::endl;
  // std::cout << "origin                 | " << glm::to_string(origin()) << std::endl;
  // std::cout << "direction              | " << glm::to_string(-glm::normalize(direction)) << std::endl;

  return direction; // direction is pointing outwards, but we want it to point towards our scene.
}

glm::vec4 DirectionalLight::origin() {
  glm::vec4 origin = glm::rotate(glm::mat4(1.0f), glm::radians(offset), glm::vec3(0.0f, 0.0f, 1.0f)) * baseOrigin;

  // Debugging
  // std::cout << "******* offset: " << offset << " ******* origin: " << glm::to_string(origin) << std::endl;

  return origin;
}

float DirectionalLight::ambientStrength(float timeOfDay) {
  if (useOverrides) {
    return ambientStrengthOverride;
  }

  float a = angle(timeOfDay);

  if (a >= illuminationEndAngle) {
    return 0.0f;
  }

  float x = a / illuminationEndAngle;
  float ambientStrength = maxAmbientStrength * (1 - pow(x, 4));

  return ambientStrength;
}

glm::vec3 DirectionalLight::ambient(float timeOfDay) {
  return glm::vec3(ambientStrength(timeOfDay));
}

float DirectionalLight::diffuseStrength(float timeOfDay) {
  if (useOverrides) {
    return diffuseStrengthOverride;
  }

  return ambientStrength(timeOfDay);
}

glm::vec3 DirectionalLight::diffuse(float timeOfDay) {
  return glm::vec3(diffuseStrength(timeOfDay));
}

glm::vec3 DirectionalLight::specular(float timeOfDay) {
  if (useOverrides) {
    return glm::vec3(specularStrengthOverride);
  }
  // Disable specular for now
  return glm::vec3(0.0f * timeOfDay);
}
