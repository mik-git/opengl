attribute vec3 inPos;
attribute vec2 inTexCoord;
uniform mat4 qt_ModelViewProjectionMatrix;
varying vec2 texCoord;

void main(void)
{
    gl_Position = qt_ModelViewProjectionMatrix * vec4(inPos,1.f);
    texCoord = inTexCoord;
}
