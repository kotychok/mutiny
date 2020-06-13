#include <iostream>

#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/array.h>
#include <mruby/string.h>

#include "lib/file.h"
#include "lib/window.h"

int main() {
  // Init mruby
  mrb_state* mrb = mrb_open();
  if (!mrb) { return 1; }

  // Load our scripts
  std::string fileContents = File::read("./src/scripts/environment.rb");
  const char* rubyCode = fileContents.c_str();
  mrb_load_string(mrb, rubyCode);
  mrb_value testValue = mrb_load_string(mrb, "ChunkGenerator.flat");
  RArray* myArray = RARRAY(testValue);
  std::cout << myArray->as.heap.len << std::endl;
  mrb_value firstElement = mrb_ary_ref(mrb, testValue, 0);
  std::cout << mrb_string_p(firstElement) << std::endl;
  mrb_p(mrb, firstElement);
  const char* myCStr = mrb_string_cstr(mrb, firstElement);
  std::cout << myCStr << std::endl;

  // Done with mruby
  mrb_close(mrb);

  Window window;
  return window.show();
}
