#pragma once

#include <mruby.h>
#include <mruby/compile.h>

/*
 * RubyVM is my wrapper over mruby.
 *
 * The spawnVM function starts a clean mruby VM then loads both Ruby-defined
 * and C++-defined methods into the VM.
 *
 * When finished with a VM, call mrb_close(mrb) where mrb is the mrb_state*.
 */
class RubyVM {
  public:
    static mrb_state* spawnVM();
};

/*
 * Whatever addition util functions I find useful
 */
mrb_value mrbext_load_and_check_string(mrb_state *mrb, const char *c_str);
