#version 410 core

// gouraud shading

in vec4 vsColor;

out vec4 fragColor;

void main()
{
    fragColor = vsColor;
}