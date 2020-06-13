#include "lib/window.h"

#include <mruby.h>
#include <mruby/compile.h>

#include "lib/file.h"

int main() {
  mrb_state *mrb = mrb_open();
  if (!mrb) { return 1; }
  std::string fileContents = File::read("./scripts/hello_world.rb");
  const char* rubyCode = fileContents.c_str();
  mrb_load_string(mrb, rubyCode);
  mrb_close(mrb);

  Window window;
  return window.show();
}
