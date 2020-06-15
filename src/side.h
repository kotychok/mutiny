#pragma once

#include <mruby.h>

enum class Side {
  NORTH,
  SOUTH,
  EAST,
  WEST,
  TOP,
  BOTTOM,
};

mrb_value mrbext_side_to_sym(mrb_state* mrb, Side side);
