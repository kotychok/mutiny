#include "lib/window.h"

#include <mruby.h>
#include <mruby/compile.h>

int main() {
  mrb_state *mrb = mrb_open();
  if (!mrb) { return 1; }
  mrb_load_string(mrb, "puts 'Hello, World!'");
  mrb_close(mrb);

  Window window;
  return window.show();
}
