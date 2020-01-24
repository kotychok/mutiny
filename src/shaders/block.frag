#version 330 core

out vec4 FragColor;

in vec3 TexCoord;

uniform sampler2DArray myTexture;

void main()
{
  FragColor = texture(myTexture, TexCoord);
}
