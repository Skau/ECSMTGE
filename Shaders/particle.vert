#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;

uniform vec3 mPos = vec3(0, 0, 0);
uniform mat4 vMatrix;
uniform mat4 pMatrix;

out vec2 uv;
out vec3 color;

void main()
{
    uv = inUV;
    color = inNormal;

    // Construct model matrix
    mat4 model = mat4(1);
    model[3].xyz = mPos;

    mat4 viewModel = mat4(1);
    viewModel[3].xyz = (vMatrix * model)[3].xyz;

    gl_Position = pMatrix * viewModel * vec4(inPos, 1.0);
}
