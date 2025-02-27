#version 410 core

layout (location = 0) in vec2 in_Position;
layout (location = 1) in vec2 in_TexCoord;

out VS_OUT
{
    vec2 texCoord;
}
vs_out;

void main()
{
    gl_Position = vec4(in_Position, 0, 1);
    vs_out.texCoord = in_TexCoord;
}