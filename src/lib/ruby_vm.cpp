#include <string>

#include <mruby.h>
#include <mruby/compile.h>

#include "file.h"
#include "noise.h"
#include "ruby_vm.h"

mrb_state* RubyVM::spawnVM() {
  // Initialize the mruby VM.
  mrb_state *mrb = mrb_open();

  // Load in our ruby application environment into the VM.
  std::string fileContents = File::read("./src/scripts/environment.rb");
  const char* rubyCode = fileContents.c_str();
  mrb_load_string(mrb, rubyCode);

  // Also add C++-defined functions to the Ruby VM.
  // Consider using mrubybind for this.
  RClass *PerlinNoise_class = mrb_define_class(mrb, "PerlinNoise", mrb->object_class);
  mrb_define_class_method(mrb, PerlinNoise_class, "get_value", Noise::getPerlinNoise, MRB_ARGS_REQ(3));

  return mrb;
}
