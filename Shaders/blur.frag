#version 330 core

in vec2 TexCoords;

uniform sampler2D fbt;
uniform ivec2 sResolution;

out vec4 FragColor;

void main()
{
    vec3 color = vec3(0, 0, 0);
    for (int y = 0; y <= 0; ++y)
    {
        for (int x = 0; x <= 0; ++x)
        {
            color += texture(fbt, TexCoords + vec2(x / float(sResolution.x), y / float(sResolution.y))).xyz;
        }
    }

    FragColor = vec4(color, 1.0);
}
