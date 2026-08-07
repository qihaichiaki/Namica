#version 330 core

in vec2 vUV;

out vec4 fColor;

uniform sampler2D uBaseTexture;
uniform vec4 uBaseColorFactor;

void main()
{
    fColor = texture(uBaseTexture, vUV) * uBaseColorFactor;
}
