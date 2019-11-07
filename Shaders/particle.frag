#version 330 core

in vec2 uv;
in vec3 color;

out vec4 fragColor;
void main()
{
    fragColor = vec4(color, 1);
}
