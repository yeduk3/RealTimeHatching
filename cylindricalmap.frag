#version 410 core

uniform sampler2D tamTexture;
uniform float radius;
uniform float height;
uniform vec2 offset;

in vec4 worldPosition;
in vec3 normal;

out vec4 fragColor;

float k = 10;

void main()
{
    vec2 texCoord;
    texCoord.x = acos((worldPosition.x - offset.x) / radius) / (2 * 3.141592 / k);
    texCoord.y = (worldPosition.y - offset.y) / height * (k / 2);
    fragColor = texture(tamTexture, texCoord);
}