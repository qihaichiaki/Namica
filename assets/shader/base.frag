#version 330 core

in vec2 vUV;
in vec3 vNormal;
in vec3 vFragPos;

out vec4 fColor;

uniform sampler2D uBaseTexture;
uniform vec4 uBaseColorFactor;

struct Light
{
    vec3 color;
    vec3 position;
};
uniform Light uLight;

void main()
{
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(uLight.position - vFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = vec4(diff * uLight.color, 1.0);

    fColor = texture(uBaseTexture, vUV) * uBaseColorFactor * diffuse;
}
