#version 330 core

out vec4 FragColor;

in vec3 Position;
in vec3 TexCoord;
in vec3 Normal;

struct Material {
  float shininess;
};

struct Light {
  // When the w component is 0,
  // "position" is actually direction.
  vec4 position;
  vec3 color;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  // Attenuation
  float constant;
  float linear;
  float quadratic;
};

uniform sampler2DArray myTexture;
uniform float ellapsedTime;
uniform vec3 cameraPosition;
uniform Material material;
uniform Light light;

void main()
{
  const float DIRECTIONAL = 0.0f;
  const float POINT = 1.0f;

  vec3 ambient = light.ambient * light.color;

  vec3 lightDir;
  if (light.position.w == POINT) {
    lightDir = normalize(vec3(light.position) - Position);
  } else if (light.position.w == DIRECTIONAL) {
    lightDir = normalize(vec3(-light.position));
  }

  float diff = max(dot(Normal, lightDir), 0.0);
  vec3 diffuse = light.diffuse * (diff * light.color);

  vec3 viewDir = normalize(cameraPosition - Position);
  vec3 reflectDir = reflect(-lightDir, Normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * (spec * light.color);

  if (light.position.w == POINT) {
    float distance = length(vec3(light.position) - Position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
  }

  vec3 result = ambient + diffuse + specular;
  FragColor = vec4(result, 1.0) * texture(myTexture, TexCoord);
}
