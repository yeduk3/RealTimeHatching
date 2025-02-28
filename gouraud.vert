#version 410 core

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;

// location

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

out vec4 worldPosition;
out vec3 normal;

// lighting

uniform vec3 lightPosition;
uniform vec3 eyePosition;
uniform vec3 lightColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float shininess;

out vec4 vsColor;

void main()
{
    vec4 worldPos = modelMat * vec4(in_Position, 1);
    gl_Position = projMat * viewMat * worldPos;
    worldPosition = worldPos;
    normal = in_Normal;

    vec3 l = lightPosition - worldPosition.xyz;
    vec3 L = normalize(l);
    vec3 N = normalize(in_Normal);
    vec3 R = 2 * dot(L, N) * N - L;
    vec3 V = normalize(eyePosition - worldPosition.xyz);
    vec3 I = lightColor / dot(l, l);

    // vec3 H = (L + V) / 2;

    vec3 ambient = diffuseColor * vec3(0.02);
    vec3 diffuse = I * diffuseColor * max(dot(L, N), 0);
    vec3 specular = I * specularColor * pow(max(dot(R, V), 0), shininess);

    vec3 color = ambient + diffuse + specular;

    color = pow(color, vec3(1 / 2.2));

    vsColor = vec4(color, 1);
}