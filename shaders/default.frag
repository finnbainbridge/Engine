#version 300 es
precision mediump float;
out vec4 FragColor;
in vec3 front_color;
in vec3 back_color;

void main()
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