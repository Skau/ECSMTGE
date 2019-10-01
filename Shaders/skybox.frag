#version 330 core

in vec3 texCoords;

uniform samplerCube cubemap;
uniform float sTime = 0.0;
uniform ivec2 sResolution = ivec2(0, 0);

out vec4 FragColor;

#define M_PI 3.1415926535897932384626433832795

float rand(float c)
{
    return fract(sin(c) * 43758.5453);
}

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float rand(vec3 co)
{
    return fract(sin(dot(co, vec3(12.9898, 78.233, 23.8221))) * 43758.5453);
}

void main()
{
    vec3 uv = (texCoords + 1.0) / 2.0;

	float starColor = 0.0;

    // infiniteMirror = (sin(uv.x * M_PI * 2.0 - M_PI / 2.0) + 1.0) * 0.5 * (sin(uv.x * 500 + sTime * 3));

    // Small stars
    int sGridCount = 300;
    float sStarThreshold = 0.98;
    vec3 grid = floor(uv * sGridCount);
    vec3 gridCentre = (grid + 0.5) / sGridCount;
    vec3 gridUv = uv * sGridCount - grid;

    float gridColor = rand(grid);
    float starAlpha = fract(gridColor);
    if (starAlpha > sStarThreshold)
    {
        float starValue = rand(starAlpha) + sTime * 0.5;
        float clampedColor = (sin(M_PI * 2.0 * (starValue - 1.0 / 4.0)) + 1.0) * 0.25 + 0.5;

        float distCircle = 1.0 - (distance(uv, gridCentre) / (0.5 / sGridCount));
        vec3 b = 1.0 - abs((gridUv - 0.5) * 4.0);
        b *= 0.7;

         float finalStar = distCircle * clampedColor * dot(b, b);
         starColor += finalStar;
    }

    // Big stars
    int bGridCount = 70;
    float bStarThreshold = 0.989;
    grid = floor(uv * bGridCount);
    gridCentre = (grid + 0.5) / bGridCount;
    gridUv = uv * bGridCount - grid;

    gridColor = rand(grid);
    starAlpha = fract(gridColor);
    if (starAlpha > bStarThreshold)
    {
        float starValue = rand(starAlpha) + sTime * 0.2;
        float clampedColor = (sin(M_PI * 2.0 * (starValue - 1.0 / 4.0)) + 1.0) * 0.25 + 0.5;

        float distCircle = 1.0 - (distance(uv, gridCentre) / (0.5 / bGridCount));
        vec3 b = 1.0 - abs((gridUv - 0.5) * 4.0);
        b *= 0.7;

        float starColorMult = 0.1f;
        float finalStar = dot(b, b) * starColorMult / abs(gridUv.x - 0.5) * starColorMult / abs(gridUv.y - 0.5) * starColorMult / abs(gridUv.z - 0.5);
        finalStar *= clampedColor * distCircle;
        starColor += finalStar;
    }

    vec3 finalColor = texture(cubemap, texCoords).rgb;
    finalColor += starColor * vec3(1, 1, 1);
    finalColor = clamp(finalColor, vec3(0, 0, 0), vec3(1, 1, 1));
	FragColor = vec4(finalColor, 1.0);
}
