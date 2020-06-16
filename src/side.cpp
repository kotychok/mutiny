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

mrb_value mrbext_side_to_sym(mrb_state *mrb, Side side) {
  mrb_value mrbSideSymbol;
  switch (side) {
    case Side::NORTH:
      mrbSideSymbol = mrb_check_intern_cstr(mrb, "north");
      break;
    case Side::SOUTH:
      mrbSideSymbol = mrb_check_intern_cstr(mrb, "south");
      break;
    case Side::EAST:
      mrbSideSymbol = mrb_check_intern_cstr(mrb, "east");
      break;
    case Side::WEST:
      mrbSideSymbol = mrb_check_intern_cstr(mrb, "west");
      break;
    case Side::TOP:
      mrbSideSymbol = mrb_check_intern_cstr(mrb, "top");
      break;
    case Side::BOTTOM:
      mrbSideSymbol = mrb_check_intern_cstr(mrb, "bottom");
      break;
  };
  return mrbSideSymbol;
}
