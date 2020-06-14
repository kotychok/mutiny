# pragma once

constexpr int CHUNK_SIZE { 32 };
constexpr int CHUNK_SIZE_CUBED { CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE };
constexpr int CHUNK_SIZE_SQUARED { CHUNK_SIZE * CHUNK_SIZE };
constexpr int CHUNK_SIZE_HALVED { CHUNK_SIZE / 2 };
constexpr int CHUNK_SIZE_QUARTERED { CHUNK_SIZE / 4 };

// I define my light position and direction vectors as vec4s.
//
// When defining position vectors as a vec4 it is important to set the w
// component to 1.0 so translation and projections are properly applied.
//
// However, when defining a direction vector as a vec4 we don't want
// translations to have an effect (since they just represent directions,
// nothing more) so then we define the w component to be 0.0.
//
// This can function as an easy check for light types: you check if the w
// component is equal to 1.0 to see that we now have a light's position vector
// and if w is equal to 0.0 we have a light's direction vector; so adjust the
// calculations based on that.
//
// https://learnopengl.com/Lighting/Light-casters
const float DIRECTIONAL_LIGHT = 0.0f;
const float POINT_LIGHT = 1.0f;
