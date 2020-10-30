#version 330 core
uniform sampler2D albedo0;
uniform sampler2D normal0;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

in vec2 texCoord;
//in vec3 normal;
in vec3 fragPos;
in mat3 TBN;

struct LightDirect {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct LightPos {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct Lamp {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct Material {
  float specularExponent;
  vec3 ambientColor;
  vec3 diffuseColor;
  vec3 specularColor;
};

uniform LightDirect lightDir;
#define NR_POINT_LIGHTS 4
uniform LightPos pointLights[NR_POINT_LIGHTS];

uniform Lamp lamp;
uniform Material material;

out vec4 FragColor;

vec3 addDirLight(LightDirect light, vec3 normal, vec3 viewDir);
vec3 addPosLight(LightPos light, vec3 normal, vec3 viewDir);
vec3 addLamp(Lamp light, vec3 normal);

void main(void)
{
//  vec3 norm = normalize(normal);
  vec3 norm = texture2D(normal0, texCoord.st).rgb;
  norm = norm * 2.0 - 1.0;
  norm = normalize(TBN * norm);
  vec3 viewDir = normalize(viewPos - fragPos);
  vec3 result = vec3(0.0f) ;
  result += addDirLight(lightDir, norm, viewDir);

  for( int i = 0; i < NR_POINT_LIGHTS ; i++) {
    result += addPosLight(pointLights[i], norm, viewDir);
  }
  result += addLamp(lamp, norm);
  FragColor = vec4(result, 1.0f);
}

vec3 addDirLight(LightDirect light, vec3 normal, vec3 viewDir)
{
  vec3 lightDir = normalize(-light.direction);

  float diff = max(dot(normal, lightDir), 0.0);
  vec3 halfwayDir = normalize(lightDir + viewDir);
  vec3 reflectDir = reflect(-lightDir, normal);

  float spec = pow(max(dot(normal, halfwayDir), 0.0), material.specularExponent);
  vec3 objectColor = texture2D(albedo0, texCoord.st).rgb;
//  vec3 objectColor = vec3(1.0f, 0.5f, 0.2f);
  vec3 ambient  = light.ambient  * material.ambientColor * objectColor;
  vec3 diffuse  = light.diffuse  * material.diffuseColor * diff * objectColor;
  vec3 specular = light.specular * material.specularColor * spec * objectColor;
  return (ambient + diffuse + specular);
}

vec3 addPosLight(LightPos light, vec3 normal, vec3 viewDir)
{
  vec3 lightDir = normalize(light.position - fragPos);

  float diff = max(dot(normal, lightDir), 0.0);
  vec3 halfwayDir = normalize(lightDir + viewDir);
  vec3 reflectDir = reflect(-lightDir, normal);

  float spec = pow(max(dot(normal, halfwayDir), 0.0), material.specularExponent);
  vec3 objectColor = texture2D(albedo0, texCoord.st).rgb;
//  vec3 objectColor = vec3(1.0f, 0.5f, 0.2f);
  // Затухание
  float distance    = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

  vec3 ambient  = light.ambient  * material.ambientColor * objectColor;
  vec3 diffuse  = light.diffuse  * material.diffuseColor * diff * objectColor;
  vec3 specular = light.specular * material.specularColor * spec * objectColor;

  ambient  *= attenuation;
  diffuse  *= attenuation;
  specular *= attenuation;

  return (ambient + diffuse + specular);
}

vec3 addLamp( Lamp light, vec3 normal)
{
  vec3 objectColor = texture2D(albedo0, texCoord.st).rgb;
//  vec3 objectColor = vec3(1.0f, 0.5f, 0.2f);
  vec3 ambient = light.ambient * material.ambientColor * objectColor;

  vec3 lightDir = normalize(light.position - fragPos);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = light.diffuse * material.diffuseColor * diff * objectColor;

  vec3 viewDir = normalize(viewPos - fragPos);
  vec3 reflectDir = reflect(-lightDir, normal);
  vec3 halfwayDir = normalize(lightDir + viewDir);
  float spec = pow(max(dot(normal, halfwayDir), 0.0), material.specularExponent);
  vec3 specular = light.specular * material.specularColor * spec * objectColor;

  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = (light.cutOff - light.outerCutOff);
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
  diffuse  *= intensity;
  specular *= intensity;

  float distance    = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
  ambient  *= attenuation;
  diffuse  *= attenuation;
  specular *= attenuation;

  return (ambient + diffuse + specular);
}
