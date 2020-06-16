#pragma once

#include <mruby.h>
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

mrb_value mrbext_side_to_sym(mrb_state* mrb, Side side);
