#version 410 core

uniform sampler2D tamTexture;

in vec2 texCoord;

out vec4 fragColor;

void main()
{
    fragColor = texture(tamTexture, texCoord);
}