#include <string>

#include <mruby.h>
#include <mruby/compile.h>

#include "constants.h"
#include "util/file.h"
#include "noise.h"
#include "ruby_vm.h"
#include "texture.h"

mrb_state* RubyVM::spawnVM() {
  //*******************************************************************
  // Initialize the mruby VM.
  //*******************************************************************
  mrb_state *mrb = mrb_open();

  //*******************************************************************
  // Load in C++-defined constants, etc which need to be loaded before our ruby
  // environment.
  //*******************************************************************
  // # Ruby style
  //
  // CHUNK_SIZE = 32
  // CHUNK_SIZE_CUBED = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
  // CHUNK_SIZE_SQUARED = CHUNK_SIZE * CHUNK_SIZE;
  // CHUNK_SIZE_HALVED = CHUNK_SIZE / 2;
  // CHUNK_SIZE_QUARTERED = CHUNK_SIZE / 4;
  //
  mrb_define_const(mrb, mrb->kernel_module, "CHUNK_SIZE", mrb_fixnum_value(CHUNK_SIZE));
  mrb_define_const(mrb, mrb->kernel_module, "CHUNK_SIZE_CUBED", mrb_fixnum_value(CHUNK_SIZE_CUBED));
  mrb_define_const(mrb, mrb->kernel_module, "CHUNK_SIZE_SQUARED", mrb_fixnum_value(CHUNK_SIZE_SQUARED));
  mrb_define_const(mrb, mrb->kernel_module, "CHUNK_SIZE_HALVED", mrb_fixnum_value(CHUNK_SIZE_HALVED));
  mrb_define_const(mrb, mrb->kernel_module, "CHUNK_SIZE_QUARTERED", mrb_fixnum_value(CHUNK_SIZE_QUARTERED));

  //*******************************************************************
  // Load in our ruby application environment into the VM.
  //*******************************************************************
  mrbext_load_and_check_string(mrb, "require './src/scripts/environment'");

  //*******************************************************************
  // Load in more C++-defined classes, methods, etc that
  //*******************************************************************
  // # Ruby style
  //
  // class CNoise
  //   def self.perlin(x, y, z); end
  // end
  //
  RClass *CNoise_class = mrb_define_class(mrb, "CNoise", mrb->object_class);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
  mrb_define_class_method(mrb, CNoise_class, "perlin", [](mrb_state *mrb, mrb_value self) -> mrb_value {
      double x {}, y {}, z {};
      mrb_get_args(mrb, "fff", &x, &y, &z);
      return mrb_float_value(mrb, Noise::perlin(x, y, z));
  }, MRB_ARGS_REQ(3));
#pragma GCC diagnostic pop

  return mrb;
}

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
