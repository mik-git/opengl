#version 330 core
uniform samplerCube skybox;
in vec3 texCoords;

out vec4 FragColor;

void main(void)
{
    FragColor = texture(skybox, texCoords);
}
