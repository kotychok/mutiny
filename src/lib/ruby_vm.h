#pragma once

#include <mruby.h>

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
