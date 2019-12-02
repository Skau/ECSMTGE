#version 330 core

in vec2 TexCoords;

uniform sampler2D fbt;
uniform float factor;

out vec4 fragColor;

void main()
{
    fragColor = vec4(texture(fbt, TexCoords).rgb * factor, 1.0);
}
