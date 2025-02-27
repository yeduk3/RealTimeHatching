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
    texCoord.y = acos((worldPosition.y - offset.y) / radius) / (3.141592 / k);
    texCoord.x = asin((worldPosition.x - offset.x) / radius / sin(3.141592 * texCoord.y / k)) / (3.141592 / k);
    fragColor = texture(tamTexture, texCoord);
}