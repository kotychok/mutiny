#version 330 core

out vec4 FragColor;

in vec3 TexCoord;
in vec3 Normal;

uniform sampler2DArray myTexture;

void main()
{
  FragColor = texture(myTexture, TexCoord);
}
