#version 330 core
uniform sampler2D texture0;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

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

uniform LightDirect lightDir;
#define NR_POINT_LIGHTS 4
uniform LightPos pointLights[NR_POINT_LIGHTS];

uniform Lamp lamp;

out vec4 FragColor;

vec3 addDirLight(LightDirect light, vec3 normal, vec3 viewDir);
vec3 addPosLight(LightPos light, vec3 normal, vec3 viewDir);
vec3 addLamp(Lamp light, vec3 normal);

void main(void)
{
  vec3 norm = normalize(normal);
  vec3 viewDir = normalize(viewPos - fragPos);
  vec3 result = vec3(0.0f) ;
  result += addDirLight(lightDir, norm, viewDir);

  for( int i = 0; i < NR_POINT_LIGHTS ; i++) {
    result += addPosLight(pointLights[i], norm, viewDir);
  }
  result += addLamp(lamp, normal);
  FragColor = vec4(result, 1.0f);
}

vec3 addDirLight(LightDirect light, vec3 normal, vec3 viewDir)
{
  vec3 lightDir = normalize(-light.direction);

  float diff = max(dot(normal, lightDir), 0.0);
  vec3 halfwayDir = normalize(lightDir + viewDir);
  vec3 reflectDir = reflect(-lightDir, normal);

  float spec = pow(max(dot(normal, halfwayDir), 0.0), 64);
  vec3 objectColor = texture2D(texture0, texCoord.st).rgb;
  vec3 ambient  = light.ambient  * objectColor;
  vec3 diffuse  = light.diffuse  * diff * objectColor;
  vec3 specular = light.specular * spec * objectColor;
  return (ambient + diffuse + specular);
}

vec3 addPosLight(LightPos light, vec3 normal, vec3 viewDir)
{
  vec3 lightDir = normalize(light.position - fragPos);

  float diff = max(dot(normal, lightDir), 0.0);
  vec3 halfwayDir = normalize(lightDir + viewDir);
  vec3 reflectDir = reflect(-lightDir, normal);

  float spec = pow(max(dot(normal, halfwayDir), 0.0), 64);
  vec3 objectColor = texture2D(texture0, texCoord.st).rgb;

  // Затухание
  float distance    = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

  vec3 ambient  = light.ambient  * objectColor;
  vec3 diffuse  = light.diffuse  * diff * objectColor;
  vec3 specular = light.specular * spec * objectColor;

  ambient  *= attenuation;
  diffuse  *= attenuation;
  specular *= attenuation;

  return (ambient + diffuse + specular);
}

vec3 addLamp( Lamp light, vec3 normal)
{
  vec3 objectColor = texture2D(texture0, texCoord.st).rgb;
  vec3 ambient = light.ambient * objectColor;

  vec3 lightDir = normalize(light.position - fragPos);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = light.diffuse * diff * objectColor;

  vec3 viewDir = normalize(viewPos - fragPos);
  vec3 reflectDir = reflect(-lightDir, normal);
  vec3 halfwayDir = normalize(lightDir + viewDir);
  float spec = pow(max(dot(normal, halfwayDir), 0.0), 64);
  vec3 specular = light.specular * spec * objectColor;

  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = (light.cutOff - light.outerCutOff);
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
  diffuse  *= intensity;
  specular *= intensity;

  float distance    = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
  ambient  *= attenuation;
  diffuse   *= attenuation;
  specular *= attenuation;

  return (ambient + diffuse + specular);
}
