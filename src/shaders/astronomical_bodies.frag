#version 330 core

out vec4 FragColor;

in vec3 Color;
in vec2 TexCoord;

uniform sampler2DArray astronomicalBodiesTexturesArray;
uniform float textureIndex;

void main() {
  // FragColor = vec4(Color, 1.0f);
  FragColor = texture(astronomicalBodiesTexturesArray, vec3(TexCoord, textureIndex));
}
