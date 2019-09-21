#version 330 core

in vec3 texCoords;

uniform samplerCube cubemap;

out vec4 FragColor;

void main()
{

    FragColor = vec4(texture(cubemap, texCoords).rgb, 1.0);
    // FragColor = vec4(1, 0, 0, 1);
}
