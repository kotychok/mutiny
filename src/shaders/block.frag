#version 330 core

out vec4 FragColor;

in vec3 Position;
in vec3 TexCoord;
in vec3 Normal;

struct Material {
  float shininess;
};

struct Light {
  vec3 position;
  vec3 color;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

uniform sampler2DArray myTexture;
uniform float ellapsedTime;
uniform vec3 cameraPosition;
uniform Material material;
uniform Light light;

void main()
{
  vec3 ambient = light.ambient * light.color;

  vec3 lightDir = normalize(light.position - Position);
  float diff = max(dot(Normal, lightDir), 0.0);
  vec3 diffuse = light.diffuse * (diff * light.color);

  vec3 viewDir = normalize(cameraPosition - Position);
  vec3 reflectDir = reflect(-lightDir, Normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * (spec * light.color);

  vec3 result = ambient + diffuse + specular;

  FragColor = vec4(result, 1.0) * texture(myTexture, TexCoord);
}
