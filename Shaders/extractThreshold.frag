#version 330 core

in vec2 TexCoords;

uniform sampler2D fbt;
uniform float threshold = 1.0;

out vec4 fragColor;

void main()
{
    vec3 col = texture(fbt, TexCoords).rgb;
    col = max(col - threshold, 0.0);
    fragColor = vec4(col, 1.0);
}
