#version 330 core
// important for VM export MESA_GL_VERSION_OVERRIDE=3.3
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 texCoord;
out vec3 fragPos;
out vec3 nNormal;
out mat3 TBN;

void main(void)
{
    gl_Position = projection * view * model * vec4(inPos,1.f);
    fragPos = vec3(model * vec4(inPos, 1.0f));
    texCoord = inTexCoord;
    vec3 normal = mat3( transpose( inverse(model) ) ) * inNormal;
    nNormal = normal;
    vec3 T = normalize(vec3(model * vec4(inTangent, 0.0f)));
    vec3 B = normalize(vec3(model * vec4(inBitangent, 0.0f)));
    vec3 N = normalize(vec3(model * vec4(inNormal, 0.0f)));
    TBN = mat3(T, B, N);


}
