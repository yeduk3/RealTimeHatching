#version 410 core

// textures

uniform sampler2D tam0;
uniform sampler2D tam1;
uniform sampler2D tam2;
uniform sampler2D tam3;
uniform sampler2D tam4;
uniform sampler2D tam5;
const float t[7] = float[7](0.0, float(1) / 6, float(2) / 6, float(3) / 6, float(4) / 6, float(5) / 6, 1.0);

in vec4 worldPosition;
in vec3 normal;

// texture repeat parameter
float k = 10;

// lighting

uniform vec3 lightPosition;
uniform vec3 eyePosition;
uniform vec3 lightColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float shininess;

out vec4 fragColor;

void main()
{
    // phong

    vec3 l = lightPosition - worldPosition.xyz;
    vec3 L = normalize(l);
    vec3 N = normalize(normal);
    vec3 R = 2 * dot(L, N) * N - L;
    vec3 V = normalize(eyePosition - worldPosition.xyz);
    vec3 I = lightColor / dot(l, l);

    vec3 ambient = diffuseColor * vec3(0.02);
    vec3 diffuse = I * diffuseColor * max(dot(L, N), 0);
    vec3 specular = I * specularColor * pow(max(dot(R, V), 0), shininess);

    vec3 color = ambient + diffuse + specular;

    color = pow(color, vec3(1 / 2.2));

    // hatching

    float tone = color.x; // 0 dark, 1 light

    vec2 texCoord;
    texCoord.y = asin(N.y) / 3.141592 * k;
    texCoord.x = atan(N.x, N.z) / 3.141592 * k;

    vec3 color1, color2;

    color1.r = texture(tam0, texCoord).r;
    color1.g = texture(tam1, texCoord).g;
    color1.b = texture(tam2, texCoord).b;
    color2.r = texture(tam3, texCoord).r;
    color2.g = texture(tam4, texCoord).g;
    color2.b = texture(tam5, texCoord).b;

    float tone2 = 1 - tone;

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

    vec3 blended = 1 - vec3(dot(1 - color1, ratio1) + dot(1 - color2, ratio2));

    fragColor = vec4(blended, 1);
    // fragColor = vec4(((texCoord/k)+1)/2, 0, 1);
}