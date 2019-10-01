#version 330 core

in vec3 texCoords;
in vec3 normal;
in vec2 uv2d;

uniform samplerCube cubemap;
uniform float sTime = 0.0;
uniform ivec2 sResolution = ivec2(0, 0);

out vec4 FragColor;

#define M_PI 3.1415926535897932384626433832795

// Simplex 2D noise
// @see https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
vec3 permute(vec3 x) { return mod(((x*34.0)+1.0)*x, 289.0); }

float simplexNoise(vec2 v){
  const vec4 C = vec4(0.211324865405187, 0.366025403784439,
           -0.577350269189626, 0.024390243902439);
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);
  vec2 i1;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;
  i = mod(i, 289.0);
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
  + i.x + vec3(0.0, i1.x, 1.0 ));
  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy),
    dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;
  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}

//	Simplex 3D Noise
//	by Ian McEwan, Ashima Arts
// @see https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}

float simplexNoise(vec3 v){
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //  x0 = x0 - 0. + 0.0 * C
  vec3 x1 = x0 - i1 + 1.0 * C.xxx;
  vec3 x2 = x0 - i2 + 2.0 * C.xxx;
  vec3 x3 = x0 - 1. + 3.0 * C.xxx;

// Permutations
  i = mod(i, 289.0 );
  vec4 p = permute( permute( permute(
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients
// ( N*N points uniformly over a square, mapped onto an octahedron.)
  float n_ = 1.0/7.0; // N=7
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z *ns.z);  //  mod(p,N*N)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
                                dot(p2,x2), dot(p3,x3) ) );
}

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float rand(vec3 co)
{
    return fract(sin(dot(co, vec3(12.9898, 78.233, 23.8221))) * 43758.5453);
}

vec2 uv3dTo2d(vec3 uv, vec3 normal)
{
    float xDot = dot(normal, vec3(1, 0, 0));
    float yDot = dot(normal, vec3(0, 1, 0));
    float zDot = dot(normal, vec3(0, 0, 1));

    float test = dot(normal, vec3(1, 0, 0));
    if (normal.x > 0.99)
    {
        return uv.zy;
    }
    else if (normal.x < -0.99)
    {
        return vec2(1.0 - uv.z, uv.y);
    }

    if (normal.y > 0.99)
    {
        return uv.xz;
    }
    else if (normal.y < -0.99)
    {
        return vec2(1.0 - uv.x, uv.z);
    }

    if (normal.z > 0.99)
    {
        return vec2(1.0 - uv.x, uv.y);
    }
    else if (normal.z < -0.99)
    {
        return uv.xy;
    }

    return vec2(0, 0);
}

// Gradient noise implementation made by Inigo Quilez - iq/2013
// @see https://www.shadertoy.com/view/XdXGW8
vec2 hash( vec2 x )
{
    const vec2 k = vec2( 0.3183099, 0.3678794 );
    x = x*k + k.yx;
    return -1.0 + 2.0*fract( 16.0 * k*fract( x.x*x.y*(x.x+x.y)) );
}

float noise( in vec2 p )
{
    vec2 i = floor( p );
    vec2 f = fract( p );

	vec2 u = f*f*(3.0-2.0*f);

    return mix( mix( dot( hash( i + vec2(0.0,0.0) ), f - vec2(0.0,0.0) ),
                     dot( hash( i + vec2(1.0,0.0) ), f - vec2(1.0,0.0) ), u.x),
                mix( dot( hash( i + vec2(0.0,1.0) ), f - vec2(0.0,1.0) ),
                     dot( hash( i + vec2(1.0,1.0) ), f - vec2(1.0,1.0) ), u.x), u.y);
}

// Simple and very unefficient 3d simplex noise for a cube.
float simple3DSimplexNoise(vec3 v)
{
    // Triangle
    float AB = simplexNoise(v.xy);
    float AC = simplexNoise(v.xz);
    float BC = simplexNoise(v.yz);

    // Inverse triangle
    float BA = simplexNoise(v.yx);
    float CA = simplexNoise(v.zx);
    float CB = simplexNoise(v.zy);

    float ABC = AB + AC + BC + BA + CA + CB;
    return ABC / 6;
}

void main()
{
    float size = 30.0;
	float prob = 0.95;

    vec3 uv = (texCoords + 1.0) / 2.0;

    vec3 pos = texCoords;
	vec3 center = floor(size * texCoords) / size;

	float color = 0.0;
	float starValue = pos.x; //rand(pos);

    // infiniteMirror = (sin(uv.x * M_PI * 2.0 - M_PI / 2.0) + 1.0) * 0.5 * (sin(uv.x * 500 + sTime * 3));

    int gridCount = 100;
    float starThreshold = 0.9;
    vec3 grid = floor(uv * gridCount);
    vec3 gridCentre = (grid + 0.5) / gridCount;
    vec3 gridUv = uv * gridCount - grid;


    // float timeMult =  + sTime * 0.01;


    float gridColor = rand(grid);
    // if (gridColor > starThreshold)
    // {
    float risingColor = fract(gridColor + sTime * 0.01);
    if (risingColor > starThreshold)
    {
        float normalizedRange = (risingColor - starThreshold) / (1 - starThreshold);
        float clampedColor = (sin(M_PI * 2.0 * (normalizedRange - 1.0 / 4.0)) + 1.0) * 0.5;

        // color = clampedColor;

        float distCircle = 1.0 - (distance(uv, gridCentre) / (0.5 / gridCount));
        vec3 b = 1.0 - abs((gridUv - 0.5) * 4.0);
        b *= 0.7;

        float starColorMult = 0.1f;
        color = dot(b, b) * starColorMult / abs(gridUv.x - 0.5) * starColorMult / abs(gridUv.y - 0.5) * starColorMult / abs(gridUv.z - 0.5);
        color *= clampedColor * distCircle;
    }



	FragColor = vec4(vec3(color), 1.0);
}
