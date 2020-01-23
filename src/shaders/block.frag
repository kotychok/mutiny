#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform int layerIndex;
uniform sampler2DArray myTexture;

void main()
{
  FragColor = texture(myTexture, vec3(TexCoord.x, TexCoord.y, 0));
}
