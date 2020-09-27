#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aTexCoords;

out vec2 texCoord;
out vec3 normals;

uniform mat4 global_transform;
uniform mat4 local_transform;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    gl_Position = projection * view * global_transform * local_transform * vec4(aPos, 1.0);
    texCoord = aTexCoords;
    normals = aNormals;
}