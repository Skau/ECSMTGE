#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform vec4 color;
uniform sampler2D diffuse;
uniform sampler2D specular;
uniform bool usingTextures = false;
uniform bool specularProvided = true;

void main()
{
    gPosition = FragPos;

    gNormal = normalize(Normal);

    gAlbedoSpec = vec4(1, 0, 0, 1);
}
