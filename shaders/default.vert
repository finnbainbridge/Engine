#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aTexCoords;

out vec2 tex_coord;
out vec3 normal;
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
} light;

// Material uniforms
uniform struct MaterialInfo
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
} material;

vec3 phongModel(vec4 cam_coords, vec3 n)
{
    vec3 s = normalize(vec3((view * light.position) - cam_coords));
    vec3 ambient = light.ambient * material.ambient;
    float sDotN = max( dot(s, n), 0.0);
    vec3 diffuse = light.diffuse * material.diffuse * sDotN;
    vec3 spec = vec3(0.0);

    if (sDotN > 0.0)
    {
        vec3 v = normalize(-cam_coords.xyz);
        vec3 r = reflect(-s, n);
        spec = light.specular * material.specular * 
                    pow(max(dot(r, v), 0.0), material.shininess);
    }

    // Calculate intensity with diffuse equation
    return ambient + diffuse + spec;
}

void main()
{
    // Convert normal and position to eye coordinates
    vec3 n = normalize(mat3(model_view) * aNormals);
    vec4 cam_coords = model_view * vec4(aPos, 1.0);
    
    front_color = phongModel(cam_coords, n);
    back_color = phongModel(cam_coords, -n);

    gl_Position = projection * cam_coords;
    tex_coord = aTexCoords;
    normal = aNormals;
}