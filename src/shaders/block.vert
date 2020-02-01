#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec3 Position;
out vec3 TexCoord;
out vec3 Normal;
out vec4 PositionInLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
   Position = vec3(model * vec4(aPos, 1.0));
   TexCoord = aTexCoord;
   Normal = normalize(aNormal);
   PositionInLightSpace = lightSpaceMatrix * vec4(Position, 1.0);

   gl_Position = projection * view * model * vec4(aPos, 1.0);
}
