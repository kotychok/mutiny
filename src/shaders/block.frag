#version 330 core

out vec4 FragColor;

in vec3 Position;
in vec3 TexCoord;
in vec3 Normal;

uniform sampler2DArray myTexture;
uniform float ellapsedTime;
uniform vec3 cameraPosition;

void main()
{
  vec3 lightPosition = vec3(16.0 * sin(ellapsedTime), 94.0f, 0.0f);
  vec3 lightColor = vec3(1.0, 1.0, 1.0);

  float ambientStrength = 0.1;
  vec3 ambient = ambientStrength * lightColor;

  vec3 lightDir = normalize(lightPosition - Position);
  float diff = max(dot(Normal, lightDir), 0.0);
  vec3 diffuse = diff * lightColor;

  float specularStrength = 0.5;
  vec3 viewDir = normalize(cameraPosition - Position);
  vec3 reflectDir = reflect(-lightDir, Normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = specularStrength * spec * lightColor;

  vec3 result = ambient + diffuse + specular;

  FragColor = vec4(result, 1.0) * texture(myTexture, TexCoord);
}
