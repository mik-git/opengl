#version 330 core
// important for VM export MESA_GL_VERSION_OVERRIDE=3.3
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;

void main(void)
{
    gl_Position = projection * view * model * vec4(inPos,1.f);
    fragPos = vec3(model * vec4(inPos, 1.0f));
    texCoord = inTexCoord;
    normal = mat3( transpose( inverse(model) ) ) * inNormal;
}
