#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;

uniform mat4 vMatrix;
uniform mat4 pMatrix;

out vec3 texCoords;

void main()
{
    texCoords = inPos;

    gl_Position = pMatrix * vMatrix * vec4(inPos, 1);
}
