#version 300 es

// If these cause problems, change
precision highp float;
precision highp int;
precision lowp sampler2D;
precision lowp samplerCube;

#define PI 3.1415926535897932384626433832795

// precision mediump float;
// precision mediump int;
// precision lowp sampler2D;
// precision lowp samplerCube;

out vec4 FragColor;

in vec3 front_color;
in vec3 back_color;

in vec3 n;
in vec4 cam_coords;

// Position uniforms
// uniform mat4 global_transform;
// uniform mat4 local_transform;
uniform mat4 transform;
uniform mat3 normal_transform;
uniform mat4 model_view;
// uniform mat3 normal_global;
// uniform mat3 normal_local;
uniform mat4 view;
uniform mat4 projection;

// lighting uniforms
uniform struct LightInfo
{
    vec4 position;
    vec3 ambient;
    vec3 intensity;
    int radius;
    int exists;
    float attenv;
}[32] light;

uniform int shading_mode;

// Material uniforms
uniform struct MaterialInfo
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;

    float rough;
    int metal;
    vec3 color;
} material;

float ggxDistribution(float nDotH)
{
    float alpha2 = material.rough * material.rough * material.rough * material.rough;
    float d = (nDotH * nDotH) * (alpha2 - 1.0) + 1.0;
    return alpha2 / (PI * d * d);
}

float geomSmith(float dotProd)
{
    float k = (material.rough + 1.0) * (material.rough + 1.0) / 8.0;
    float denom = dotProd * (1.0 - k) + k;
    return 1.0 / denom;
}

vec3 schlickFresnel(float lDotH)
{
    vec3 f0 = vec3(0.04); // Dielectrics
    if (material.metal == 1)
    {
        f0 = material.color;
    }

    return f0 + (vec3(1.0) - f0) * pow(1.0 - lDotH, 5.0);
}

vec3 microfacetModel( int lightIdx, vec3 position, vec3 n ) {
    vec3 diffuseBrdf = vec3(0.0); // Metallic
    if( material.metal == 0 ) {
        diffuseBrdf = material.color;
    }
    vec3 l = vec3(0.0),
    lightI = light[lightIdx].intensity;
    // if( light[lightIdx].position.w == 0.0 ) { // Directional light
    //     l = normalize(light[lightIdx].position.xyz);
    // } else {
        // Positional light
        l = vec3(view * light[lightIdx].position) - position;
        float dist = length(l);
        l = normalize(l);
        lightI /= (dist * dist);
    // }
    vec3 v = normalize( -position );
    vec3 h = normalize( v + l );
    float nDotH = dot( n, h );
    float lDotH = dot( l, h );
    float nDotL = max( dot( n, l ), 0.0 );
    float nDotV = dot( n, v );
    vec3 specBrdf = 0.25 * ggxDistribution(nDotH) *
    schlickFresnel(lDotH) * geomSmith(nDotL) *
    geomSmith(nDotV);
    return (diffuseBrdf + PI * specBrdf) * lightI * nDotL;
}

void main()
{
    vec3 sum = vec3(0.0);

    for (int i = 0; i < 32; i++)
    {
        if (light[i].exists == 1)
        {
            sum += microfacetModel(i, cam_coords.xyz, n) * light[i].attenv;
        }
    }

    // Gamma
    sum = pow(sum, vec3(1.0/2.2));
    FragColor = vec4(sum, 1);
}