#pragma once

#include <mruby.h>

#include "../side.h"

mrb_value mrbext_side_to_sym(mrb_state* mrb, Side side);
