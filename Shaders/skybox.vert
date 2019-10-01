#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;

uniform mat4 vMatrix;
uniform mat4 pMatrix;

out vec3 texCoords;
out vec3 normal;
out vec2 uv2d;

void main()
{
    texCoords = normalize(inPos);
    normal = inNormal;
    uv2d = normalize(inPos.xy);

    vec4 pos = pMatrix * mat4(mat3(vMatrix)) * vec4(inPos, 1);
    gl_Position = pos.xyww;
}
