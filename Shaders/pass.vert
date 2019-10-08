#version 330 core

layout (location = 0) in vec3 inPos;

out vec2 TexCoords;

void main()
{
    TexCoords = (inPos.xy + 1.0) / 2.0;
    gl_Position = vec4(inPos, 1.0);
}
