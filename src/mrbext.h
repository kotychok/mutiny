#pragma once

#include <mruby.h>

mrb_value mrbext_load_and_check_string(mrb_state *mrb, const char *c_str);
