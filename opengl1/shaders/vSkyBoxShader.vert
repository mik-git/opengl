#version 330 core
// important for VM export MESA_GL_VERSION_OVERRIDE=3.3
layout (location = 0) in vec3 inPos;

out vec3 texCoords;

uniform mat4 view;
uniform mat4 projection;

void main(void)
{
  texCoords = inPos;
  gl_Position = projection * mat4(mat3(view)) * vec4(inPos,1.f);
}

