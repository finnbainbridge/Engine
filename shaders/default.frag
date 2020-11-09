#version 300 es

// If these cause problems, change
precision highp float;
precision highp int;
precision lowp sampler2D;
precision lowp samplerCube;

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

// Lighting uniforms
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
} material;

// vec3 phongModel(int l, vec4 cam_coords, vec3 n)
// {
//     vec3 s = normalize(vec3((view * light[l].position) - cam_coords));
//     vec3 ambient = light[l].ambient * material.ambient;
//     float sDotN = max( dot(s, n), 0.0);
//     vec3 diffuse = light[l].diffuse * material.diffuse * sDotN;
//     vec3 spec = vec3(0.0);

//     if (sDotN > 0.0)
//     {
//         vec3 v = normalize(-cam_coords.xyz);
//         vec3 r = reflect(-s, n);
//         spec = light[l].specular * material.specular * 
//                     pow(max(dot(r, v), 0.0), material.shininess);
//     }

//     // Calculate intensity with diffuse equation
//     return ambient + diffuse + spec;
// }

vec3 blinnPhong(int l, vec4 cam_coords, vec3 normal, int backface)
{
    vec3 ambient = light[l].ambient * material.ambient;
    vec3 s = normalize(vec3((view * light[l].position) - cam_coords));

    float sDotN = max( dot(s, normal), 0.0);
    vec3 diffuse = material.diffuse * sDotN;
    vec3 spec = vec3(0.0);

    if (sDotN > 0.0)
    {
        vec3 v = vec3(0.0);
        if (backface == 1)
        {
            v = normalize(cam_coords.xyz);
        }
        else
        {
            v = normalize(-cam_coords.xyz);
        }

        vec3 h = normalize(v + s);
        spec = material.specular * pow(max(dot(h,n), 0.0), material.shininess);
    }

    // Maybe change brackets, idk
    return (ambient + (light[l].intensity)) * (diffuse + spec);
}

// vec3 blinnPhong(int l, vec4 cam_coords, vec3 n)
// {
//     vec3 s = normalize(vec3((view * light[l].position) - cam_coords));
//     vec3 ambient = light[l].ambient * material.ambient;
//     float sDotN = max( dot(s, n), 0.0);
//     vec3 diffuse = light[l].intensity * material.diffuse * sDotN;
//     vec3 spec = vec3(0.0);

//     if (sDotN > 0.0)
//     {
//         vec3 v = normalize(-cam_coords.xyz);
//         vec3 h = normalize(v + s);
//         spec = material.specular * pow(max(dot(h,n), 0.0), material.shininess);
//     }

//     // Calculate intensity with diffuse equation
//     return ambient + diffuse + spec;
// }

void main()
{
    if (shading_mode == 0)
    {
        if (gl_FrontFacing)
        {
            FragColor = vec4(front_color, 1.0f);
        }
        else
        {
            FragColor = vec4(back_color, 1.0f);
        }
    }
    else
    {
        if (gl_FrontFacing)
        {
            vec3 outc = vec3(0.0);
            int done = 0;
            for (int i = 0; i < 32; i++)
            {
                if (light[i].exists == 1)
                {
                    outc += blinnPhong(i, cam_coords, normalize(n), 0) * light[i].attenv;
                    // outc += blinnPhong(i, cam_coords, normalize(-n), 1) * light[i].attenv;
                    done = 1;
                }
            }

            if (done == 0)
            {
                outc = material.ambient;
            }
            FragColor = vec4(outc, 1);
        }
        else
        {
            vec3 outc = vec3(0.0);
            int done = 0;
            for (int i = 0; i < 32; i++)
            {
                if (light[i].exists == 1)
                {
                    outc += blinnPhong(i, cam_coords, normalize(-n), 0) * light[i].attenv;
                    // outc += blinnPhong(i, cam_coords, normalize(n), 0) * light[i].attenv;
                    done = 1;
                }
            }

            if (done == 0)
            {
                outc = material.ambient;
                // outc = vec3(1, 0, 0);
            }
            FragColor = vec4(outc, 1);
        }
    }
}