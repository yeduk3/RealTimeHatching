#version 410 core

out vec4 fragColor;

uniform sampler2D strokeTex;
in vec2 texCoord;

void main()
{
    // fragColor = vec4(0, 0, 0, 1);
    fragColor = texture(strokeTex, texCoord);
}