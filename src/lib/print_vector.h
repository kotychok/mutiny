# pragma once

#include <array>
#include <iterator>
#include <ostream>
#include <vector>

#include <glm/glm.hpp>

// https://stackoverflow.com/a/19152438
std::ostream& operator<<(std::ostream& o, const glm::vec4 vec)
{
  o << "(" << vec.x << "," << vec.y << "," << vec.z << ")";
  return o;
}

std::ostream& operator<<(std::ostream& o, const glm::vec3 vec)
{
  o << "(" << vec.x << "," << vec.y << "," << vec.z << ")";
  return o;
}
