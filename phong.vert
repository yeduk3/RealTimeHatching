#version 410 core

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;

// location

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

out vec4 worldPosition;
out vec3 normal;

void main()
{
    vec4 worldPos = modelMat * vec4(in_Position, 1);
    gl_Position = projMat * viewMat * worldPos;
    worldPosition = worldPos;
    normal = in_Normal;
}