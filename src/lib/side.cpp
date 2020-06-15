#include "side.h"

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
