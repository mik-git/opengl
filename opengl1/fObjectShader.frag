#version 330 core
uniform sampler2D texture0;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

out vec4 FragColor;

void main(void)
{
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 objectColor = texture2D(texture0, texCoord.st).rgb;
    float ambient = 0.15;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}