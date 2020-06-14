#pragma once

#include <mruby.h>
#include <noise/noise.h>

class Noise {
  public:
    static mrb_value getPerlinNoise(mrb_state *mrb, mrb_value self);
  private:
    static noise::module::Perlin perlinNoise;
};

