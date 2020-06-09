#pragma once

#include <ostream>

#include "side.h"

std::ostream& operator<<(std::ostream& o, const Side side) {
  if (side == Side::NORTH) {
    o << "North";
  } else if (side == Side::SOUTH) {
    o << "South";
  } else if (side == Side::EAST) {
    o << "East";
  } else if (side == Side::WEST) {
    o << "West";
  } else if (side == Side::TOP) {
    o << "Top";
  } else if (side == Side::BOTTOM) {
    o << "Bottom";
  }
  return o;
}
