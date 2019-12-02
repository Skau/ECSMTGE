#version 330 core

in vec2 TexCoords;

uniform sampler2D fbt;
uniform float gamma = 2.2;
uniform float exposure = 1.0;

out vec4 fragColor;

void main()
{
    vec3 color = texture(fbt, TexCoords).rgb;
    // Reinhard tone mapping
    vec3 mapped = vec3(1.0) - exp(-color * exposure);
    mapped = pow(mapped, vec3(1.0 / gamma));

    fragColor = vec4(mapped, 1.0);
}
