#version 330 core
out vec4 FragColor;

in vec2 TexCoords;


struct Light {
    vec3 Position;
    vec3 Direction;
    float CutOff;
    float OuterCutOff;
    vec3 Color;

    float constant;
    float linear;
    float quadratic;
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
    
    vec3 lighting;
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // diffuse
    vec3 lightDir = normalize(light.Position - FragPos);
    vec3 diffuse = max(dot(norm, lightDir), 0.0) * Diffuse * light.Color;

    // specular
    float spec = pow(max(dot(viewDir, norm), 0.0), 16.0);
    vec3 specular = light.Color * spec * Specular;

    // Cutoff
    float theta = dot(lightDir, normalize(-light.Direction));
    float epsilon = (light.CutOff - light.OuterCutOff);
    float intensity = clamp((theta - light.OuterCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

    // Attenuation
    float distance = length(light.Position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    vec3 ambient = (Diffuse * 0.1) * attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    lighting = ambient + diffuse + specular;
    
    FragColor = vec4(lighting, 1.0);
}
