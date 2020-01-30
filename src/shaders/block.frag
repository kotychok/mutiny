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
uniform Light lights[4];

const float DIRECTIONAL = 0.0f;
const float POINT = 1.0f;

vec3 CalcPointLight(Light light, vec3 cameraDir) {

  vec3 ambient = light.ambient * light.color;

  vec3 lightDir;
  if (light.position.w == POINT) {
    lightDir = normalize(vec3(light.position) - Position);
  } else if (light.position.w == DIRECTIONAL) {
    lightDir = normalize(vec3(-light.position));
  }

  float diff = max(dot(Normal, lightDir), 0.0);
  vec3 diffuse = light.diffuse * (diff * light.color);

  vec3 reflectDir = reflect(-lightDir, Normal);
  float spec = pow(max(dot(cameraDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * (spec * light.color);

  if (light.position.w == POINT) {
    float distance = length(vec3(light.position) - Position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
  }

  return ambient + diffuse + specular;
}

void main()
{
  vec3 cameraDir = normalize(cameraPosition - Position);

  vec3 result;
  for (int i = 0; i < 4; i++) {
    result += CalcPointLight(lights[i], cameraDir);
  }
  FragColor = vec4(result, 1.0) * texture(myTexture, TexCoord);
}
