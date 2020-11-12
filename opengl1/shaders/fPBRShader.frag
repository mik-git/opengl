#version 330 core
uniform sampler2D albedo0;
uniform sampler2D normal0;
uniform sampler2D metallic0;
uniform sampler2D roughness0;
uniform sampler2D ao0;

uniform bool useAlbedoMap;
uniform bool useNormalMap;
uniform bool useMetallicMap;
uniform bool useRoughnessMap;
uniform bool useAOMap;

uniform vec3 viewPos;

in vec2 texCoord;
in vec3 fragPos;
in mat3 TBN;
in vec3 nNormal;

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
  float metallic;
  float roughness;
  float ao;
};

uniform LightDirect lightDir;
#define NR_POINT_LIGHTS 4
uniform LightPos pointLights[NR_POINT_LIGHTS];

uniform Lamp lamp;
uniform Material material;

out vec4 FragColor;

const float PI = 3.14159265359;

float distributionGGX(vec3 N, vec3 H, float roughness);
float geometrySchlickGGX(float NdotV, float roughness);
float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

vec3 addDirLightPBR(LightDirect light, vec3 N, vec3 V, vec3 albedo, float metallic, float roughness, float ao);
vec3 addPosLightPBR(vec3 N, vec3 V, vec3 albedo, float metallic, float roughness, float ao);
vec3 addLampPBR(Lamp light, vec3 V, vec3 N, vec3 albedo, float metallic, float roughness, float ao);
void main(void)
{
  vec3 norm;
  if (  useNormalMap ) {
    norm = texture2D(normal0, texCoord).rgb;
    norm = norm * 2.0 - 1.0;
    norm = normalize(TBN * norm);
  }
  else {
    norm = normalize(nNormal);
  }

  vec3 viewDir = normalize(viewPos - fragPos);

  vec3 albedo;
  if (useAlbedoMap) {
//      albedo = pow(texture2D(albedo0, texCoord).rgb, vec3(2.2));
    albedo = texture2D(albedo0, texCoord).rgb;
  }
  else {
    albedo = material.ambientColor;
  }

  float metallic;
  if ( useMetallicMap ) {
    metallic = texture2D(metallic0, texCoord).r;
  }
  else {
    metallic = material.metallic;
  }
  float roughness;
  if (useRoughnessMap) {
   roughness = texture2D(roughness0, texCoord).r;
  }
  else {
    roughness = material.roughness;
  }
  float ao;
  if ( useAOMap ) {
    ao = texture2D(ao0, texCoord).r;
  }
  else {
    ao = material.ao;
  }

  vec3 result = vec3(0.0f) ;

  //dirLight
  result += addDirLightPBR( lightDir, norm, viewDir, albedo, metallic, roughness, ao);


  //PosLight
  result += addPosLightPBR(norm, viewDir, albedo, metallic, roughness, ao);

  //Lamp
  result += addLampPBR(lamp, norm,viewDir, albedo, metallic, roughness, ao);
  FragColor = vec4(result, 1.0f);
}

vec3 addDirLightPBR(LightDirect light, vec3 N, vec3 V,vec3 albedo, float metallic, float roughness, float ao)
{
  vec3 F0 = normalize(vec3(0.04) );
  F0 = mix(F0, albedo, metallic);

  vec3 Lo = vec3(0.0);
  vec3 L = normalize( -light.direction );
  vec3 H = normalize(V + L);
  vec3 radiance = light.diffuse;

  float NDF = distributionGGX( N, H, roughness);
  float G = geometrySmith( N, V, L, roughness);
  vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

  vec3 kS = F;
  vec3 kD = vec3(1.0) - kS;
  kD *= 1.0 - metallic;

  vec3 numenator = NDF * G *F;
  float denominator = 4.0 * max( dot( N ,V), 0.0 ) * max( dot( N,L), 0.0 );

  vec3 specular = numenator / max( denominator, 0.001 );

  float NdotL = max( dot( N ,L), 0.0);
  Lo += (kD * albedo/PI + specular) * radiance * NdotL;


  vec3 ambient = vec3(0.1) * albedo * ao;
  vec3 color = ambient +  Lo;

  color = color/(color + vec3(1.0));
  color = pow(color, vec3(1.0/2.2));
  return color;
}

vec3 addPosLightPBR(vec3 N, vec3 V, vec3 albedo, float metallic, float roughness, float ao )
{
  vec3 F0 = normalize(vec3(0.04) );
  F0 = mix(F0, albedo, metallic);

  vec3 Lo = vec3(0.0);
  for ( int i = 0; i < NR_POINT_LIGHTS; ++i ) {
    vec3 L = normalize( pointLights[i].position - fragPos );
    vec3 H = normalize(V + L);
    float distance = length( pointLights[i].position - fragPos );
//    float attenuation = 1.0 / (distance * distance);
    float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + pointLights[i].quadratic * (distance * distance));
    vec3 radiance = pointLights[i].diffuse * attenuation;

    float NDF = distributionGGX( N, H, roughness);
    float G = geometrySmith( N, V, L, roughness);
    vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 numenator = NDF * G *F;
    float denominator = 4.0 * max( dot( N ,V), 0.0 ) * max( dot( N,L), 0.0 );

    vec3 specular = numenator / max( denominator, 0.001 );

    float NdotL = max( dot( N ,L), 0.0);
    Lo += (kD * albedo/PI + specular) * radiance * NdotL;
  }


  vec3 ambient = vec3(0.03) * albedo * ao;
  vec3 color = ambient +  Lo;

  color = color/(color + vec3(1.0));
//  color = pow(color, vec3(1.0/2.2));
  return color;
}

vec3 addLampPBR( Lamp light, vec3 V, vec3 N, vec3 albedo, float metallic, float roughness, float ao)
{
  vec3 F0 = normalize(vec3(0.04) );
  F0 = mix(F0, albedo, metallic);

  vec3 Lo = vec3(0.0);
  vec3 L = normalize( light.position - fragPos );
  vec3 H = normalize(V + L);
  float distance = length( light.position - fragPos );
//  float attenuation = 1.0 / (distance * distance);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
  vec3 radiance = light.diffuse * attenuation;

  float NDF = distributionGGX( N, H, roughness);
  float G = geometrySmith( N, V, L, roughness);
  vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

  vec3 kS = F;
  vec3 kD = vec3(1.0) - kS;
  kD *= (1.0 - metallic);

  vec3 numenator = NDF * G *F;
  float denominator = 4.0 * max( dot( N ,V), 0.0 ) * max( dot( N,L), 0.0 );

  vec3 specular = numenator / max( denominator, 0.001 );

  float NdotL = max( dot( N ,L), 0.0);
  Lo += (kD * albedo/PI + specular) * radiance * NdotL;

  float theta = dot(L, normalize(-light.direction));
  float epsilon = (light.cutOff - light.outerCutOff);
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
  Lo  *= intensity;

  vec3 ambient = albedo * ao * radiance * intensity;
  vec3 color = ambient +  Lo;

  color = color/(color + vec3(1.0));
//  color = pow(color, vec3(1.0/2.2));
  return color;
}

float distributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / max(denom,0.001);
}

float geometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / max(denom,0.001);
}
float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = geometrySchlickGGX(NdotV, roughness);
    float ggx1 = geometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
  cosTheta = min(cosTheta, 1.0);
  float p=pow(1.0 - cosTheta, 5.0);
  return F0 + (1.0 - F0) * p;
}
