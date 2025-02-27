#version 410 core

layout (location = 0) in vec2 in_Position;
layout (location = 1) in vec2 in_TexCoord;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

out vec2 texCoord;

void main()
{
    vec4 worldPos = modelMat * vec4(in_Position, 0, 1);
    gl_Position = projMat * viewMat * worldPos;
    texCoord = in_TexCoord;
}