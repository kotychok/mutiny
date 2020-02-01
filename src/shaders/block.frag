#version 330 core

out vec4 FragColor;

in vec3 Position;
in vec3 TexCoord;
in vec3 Normal;
in vec4 FragPosLightSpace;

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
uniform sampler2D shadowMap;
uniform Material material;
uniform Light lights[4];
uniform bool debugShadows;
uniform float shadowAcneBias;

const float DIRECTIONAL = 0.0f;
const float POINT = 1.0f;

float CalcShadow(vec4 fragPosLightSpace) {
  vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
  projCoords = projCoords * 0.5 + 0.5;
  if (projCoords.z > 1.0) {
    // If the coordinate we are checking for shadow lies outside of the light's
    // frustrum, force it to appear not in shadow.
    return 0.0;
  } else {
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float shadow = currentDepth - shadowAcneBias > closestDepth ? 1.0 : 0.0;
    return shadow;
  }
}

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

  if (light.position.w == DIRECTIONAL) {
    float shadow = CalcShadow(FragPosLightSpace);

    if (debugShadows) {
      return shadow == 1.0 ? vec3(1.0, 0.0, 0.0) : vec3(0.0, 1.0, 0.0);
    } else {
      return ambient + (1.0 - shadow) * (diffuse + specular);
    }
  } else {
    return ambient + diffuse + specular;
  }
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
