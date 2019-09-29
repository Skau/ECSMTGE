#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

struct Light {
    vec3 Direction;
    vec3 Color;
};

uniform vec3 viewPos;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform Light light;

void main()
{
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    vec3 lighting = Diffuse * 0.1;
    vec3 viewDir = normalize(viewPos - FragPos);
    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-light.Direction);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.Color;

    // specular
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
    vec3 specular = light.Color * spec * Specular;

    lighting += diffuse + specular;

    FragColor = vec4(lighting, 1.0);
}

