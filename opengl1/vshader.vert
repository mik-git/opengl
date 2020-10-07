#version 330 core
// important for VM export MESA_GL_VERSION_OVERRIDE=3.3
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTexCoord;
uniform mat4 qt_ModelViewProjectionMatrix;
out vec2 texCoord;

void main(void)
{
    gl_Position = qt_ModelViewProjectionMatrix * vec4(inPos,1.f);
    texCoord = inTexCoord;
}
