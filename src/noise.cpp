#include "noise.h"

noise::module::Perlin Noise::s_perlinNoise;

double Noise::perlin(double x, double y, double z) {
  return s_perlinNoise.GetValue(x, y, z);
}
