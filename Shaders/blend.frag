#version 330 core

in vec2 TexCoords;

uniform sampler2D fbt;
uniform sampler2D fbt2;
uniform int blendmode = 0;

out vec4 FragColor;

void main()
{
    if (blendmode == 0)
    {
        FragColor = texture(fbt, TexCoords) + texture(fbt2, TexCoords);
    }
    else if (blendmode == 1)
    {
        FragColor = texture(fbt, TexCoords) * texture(fbt2, TexCoords);
    }
}
