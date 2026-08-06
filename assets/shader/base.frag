#version 330 core

in vec2 vUV;

out vec4 fColor;

uniform sampler2D uTexture;
uniform vec4 uBaseColorFactor;

void main()
{
    fColor = texture(uTexture, vUV) * uBaseColorFactor;
}
