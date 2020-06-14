#include "noise.h"

noise::module::Perlin Noise::perlinNoise;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
mrb_value Noise::getPerlinNoise(mrb_state *mrb, mrb_value self) {
  double x {}, y {}, z {};
  mrb_get_args(mrb, "fff", &x, &y, &z);
  return mrb_float_value(mrb, Noise::perlinNoise.GetValue(x, y, z));
}
#pragma GCC diagnostic pop
