#version 410 core

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;

in vec2 texCoord;

out vec4 fragColor;

void main()
{
    vec3 color;
    color.r = vec4(1 - texture(tex0, texCoord)).r;
    color.g = vec4(1 - texture(tex1, texCoord)).g;
    color.b = vec4(1 - texture(tex2, texCoord)).b;
    fragColor = vec4(color, 1);
}