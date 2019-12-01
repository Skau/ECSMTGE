#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

struct Light {
    vec3 Position;
    vec3 Color;
    float Radius;
    float Intensity;
};

uniform vec3 viewPos;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform Light light;

float unrealFalloff( in float _fDistance, in float _fRadius ) {
	float fFalloff = 0;
	float fDistOverRadius = _fDistance / _fRadius;
	float fDistOverRadius4 = fDistOverRadius * fDistOverRadius * fDistOverRadius * fDistOverRadius;
	fFalloff = pow(clamp( 1.0 - fDistOverRadius4, 0.0, 1.0), 2);
	fFalloff /= _fDistance * _fDistance + 1.0;
	return fFalloff;
}

float falloff(in float _distance, in float _radius)
{
    float a = 1.0 / (_distance * _distance);
    float b = 1.0 / (_radius * _radius);
    return max(a - b, 0.0);
}

void main()
{
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    vec3 lighting = Diffuse * 0.1;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 norm = normalize(Normal);

    float dist = length(light.Position - FragPos);
    if(dist < light.Radius)
    {
        // diffuse
        vec3 lightDir = normalize(light.Position - FragPos);
        vec3 diffuse = max(dot(norm, lightDir), 0.0) * Diffuse * light.Color;
        // specular
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
        vec3 specular = light.Color * spec * Specular;
        // attenuation
        float attenuation = falloff(dist, light.Radius) * light.Intensity;
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += diffuse + specular;
    }

    FragColor = vec4(lighting, 1.0);
}
