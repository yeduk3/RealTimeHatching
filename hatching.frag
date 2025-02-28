#version 410 core

// textures

uniform sampler2D tam0;
uniform sampler2D tam1;
uniform sampler2D tam2;
uniform sampler2D tam3;
uniform sampler2D tam4;
uniform sampler2D tam5;

// sphere map

uniform float radius;
uniform float height;
uniform vec2 offset;

in vec4 worldPosition;
in vec3 normal;

// texture repeat parameter
float k = 10;

// tone down parameter
float td = -0.1;

// gouraud shading

in vec4 vsColor;

out vec4 fragColor;

const float t[7] = float[7](0.0, float(1) / 6, float(2) / 6, float(3) / 6, float(4) / 6, float(5) / 6, 1.0);

void main()
{
    // first - one blend ratio

    float tone = vsColor.x; // 0 dark, 1 light

    vec2 texCoord;
    texCoord.y = acos((worldPosition.y - offset.y) / radius) / 3.141592 * k;
    texCoord.x = asin((worldPosition.x - offset.x) / radius / sin(3.141592 * texCoord.y)) / 3.141592 * k;

    vec3 color1, color2;
    color1.r = texture(tam0, texCoord).r;
    color1.g = texture(tam2, texCoord).g;
    color1.b = texture(tam4, texCoord).b;
    color2.r = texture(tam1, texCoord).r;
    color2.g = texture(tam3, texCoord).g;
    color2.b = texture(tam5, texCoord).b;

    vec3 blendRatio;
    blendRatio.r = tone * pow(sin(3.141592 / 2 * tone), 2);
    blendRatio.b = (1 - tone) * pow(cos(3.141592 / 2 * tone), 2);
    blendRatio.g = 1 - blendRatio.r - blendRatio.b;

    vec3 blended1 = vec3(dot(color1, blendRatio) + dot(color2, blendRatio));

    // second - two blend ratio

    color1.r = texture(tam0, texCoord).r;
    color1.g = texture(tam1, texCoord).g;
    color1.b = texture(tam2, texCoord).b;
    color2.r = texture(tam3, texCoord).r;
    color2.g = texture(tam4, texCoord).g;
    color2.b = texture(tam5, texCoord).b;

    float tone2 = 1 - tone + td;

    int i;
    for (i = 1; i < 7; i++)
    {
        if (tone2 < t[i])
            break;
    }
    float right = (tone2 - t[i - 1]) * 6; // right
    float left = 1.0 - right;
    float ratioArray[7] = float[7](0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    ratioArray[i - 1] = left;
    ratioArray[i] = right;

    vec3 ratio1 = vec3(ratioArray[1], ratioArray[2], ratioArray[3]);
    vec3 ratio2 = vec3(ratioArray[4], ratioArray[5], ratioArray[6]);

    vec3 blended2 = 1 - vec3(dot(1 - color1, ratio1) + dot(1 - color2, ratio2));

    // fragColor = vsColor;
    fragColor = vec4(blended2, 1);
    // fragColor = vec4(blendRatio, 1);
    // fragColor = texture(tam5, texCoord);
}