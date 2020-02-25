#version 330 core

out vec4 FragColor;

in vec3 Color;
in vec3 TexCoord;

uniform sampler2DArray astronomicalBodiesTexturesArray;

void main() {
  // FragColor = vec4(Color, 1.0f);
  FragColor = texture(astronomicalBodiesTexturesArray, TexCoord);
}
