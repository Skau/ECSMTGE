#version 330 core

in vec2 TexCoords;

uniform sampler2D fbt;
uniform bool horizontal;

// uniform int radius = 1;

uniform float threshold = 1.0;

out vec4 FragColor;

void main()
{
    const float weights[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
    ivec2 texSize = textureSize(fbt, 0);

    vec3 color = vec3(0, 0, 0);
    vec2 offset;
    if (horizontal)
        offset = vec2(1.0 / texSize.x, 0.0);
    else
        offset = vec2(0.0, 1.0 / texSize.y);

    for (int i = -4; i < 5; ++i)
        color += texture(fbt, TexCoords + offset * i).rgb * weights[(i < 0) ? -i : i];

    FragColor = vec4(color, 1.0);
}
