#pragma once

#include <ostream>

enum class Side {
  NORTH,
  SOUTH,
  EAST,
  WEST,
  TOP,
  BOTTOM,
};

std::ostream& operator<<(std::ostream& o, const Side side);
