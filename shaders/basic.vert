#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aTexCoords;

out vec2 tex_coord;
out vec3 normal;

// Position uniforms
uniform mat4 global_transform;
uniform mat4 local_transform;
uniform mat3 normal_global;
uniform mat3 normal_local;
uniform mat4 view;
uniform mat4 projection;

// Lighting uniforms
uniform vec4 light_position;
uniform vec3 light_intensity; // Intensity

// Material uniforms
uniform vec3 diffuse; // Diffuse


void main()
{
    gl_Position = projection * view * global_transform * local_transform * vec4(aPos, 1.0);
    tex_coord = aTexCoords;
    normal = aNormals;
}