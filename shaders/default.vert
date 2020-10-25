#version 300 es
precision highp float;
precision highp int;
precision lowp sampler2D;
precision lowp samplerCube;
// precision mediump float;
// precision mediump int;
// precision lowp sampler2D;
// precision lowp samplerCube;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aTexCoords;

out vec2 tex_coord;
out vec3 normal;

out vec3 n;
out vec4 cam_coords;

out vec3 front_color;
out vec3 back_color;

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

// Lighting uniforms
uniform struct LightInfo
{
    vec4 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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
} material;

vec3 phongModel(int l, vec4 cam_coords, vec3 n)
{
    vec3 s = normalize(vec3((view * light[l].position) - cam_coords));
    vec3 ambient = light[l].ambient * material.ambient;
    float sDotN = max( dot(s, n), 0.0);
    vec3 diffuse = light[l].diffuse * material.diffuse * sDotN;
    vec3 spec = vec3(0.0);

    if (sDotN > 0.0)
    {
        vec3 v = normalize(-cam_coords.xyz);
        vec3 r = reflect(-s, n);
        spec = light[l].specular * material.specular * 
                    pow(max(dot(r, v), 0.0), material.shininess);
    }

    // Calculate intensity with diffuse equation
    return ambient + diffuse + spec;
}

void main()
{
    // Convert normal and position to eye coordinates
    n = normalize(mat3(model_view) * aNormals);
    cam_coords = model_view * vec4(aPos, 1.0);
    
    if (shading_mode == 0)
    {
        front_color = vec3(0.0);
        back_color = vec3(0.0);

        int happened = 0;
        for (int i = 0; i < 32; i++)
        {
            if (light[i].exists == 1)
            {
                float attenv = 1.0 - distance(transform * vec4(0, 0, 0, 1), light[i].position)/float(light[i].radius);
                front_color += phongModel(i, cam_coords, n) * attenv;
                back_color += phongModel(i, cam_coords, -n) * attenv;
                happened = 1;
            }
        }

        if (happened == 0)
        {
            front_color = material.ambient;
            back_color = material.ambient;
        }
    }

    gl_Position = projection * cam_coords;
    tex_coord = aTexCoords;
    normal = aNormals;
}