#pragma once

#include <mruby.h>
#include <noise/noise.h>

class Noise {
  public:
    static double getPerlinNoise(double x, double y, double z);
  private:
    static noise::module::Perlin s_perlinNoise;
};

