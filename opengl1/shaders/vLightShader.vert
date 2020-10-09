#version 330 core
// important for VM export MESA_GL_VERSION_OVERRIDE=3.3
layout (location = 0) in vec3 inPos;
uniform mat4 qt_ModelViewProjectionMatrix;

void main(void)
{
    gl_Position = qt_ModelViewProjectionMatrix * vec4(inPos,1.f);
}

