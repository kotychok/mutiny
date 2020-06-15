#include <cstdlib>

#include <mruby/compile.h>

#include "mrbext.h"

mrb_value mrbext_load_and_check_string(mrb_state *mrb, const char *code) {
  mrb_value mrbResult = mrb_load_string(mrb, code);
  if (mrb->exc) { // If there is an error
    if (!mrb_undef_p(mrbResult)) {
      mrb_print_error(mrb); // print exception object
      exit(1);
    }
  }
  return mrbResult;
}
