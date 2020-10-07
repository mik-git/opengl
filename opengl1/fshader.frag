#version 330 core
uniform mediump sampler2D texture0;
in vec2 texCoord;
out vec4 FragColor;

void main(void)
{
    FragColor = texture2D(texture0, texCoord.st);
}
