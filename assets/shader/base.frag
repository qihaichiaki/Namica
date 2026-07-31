#version 330 core

in vec2 vUV;

out vec4 fColor;

uniform sampler2D uTexture;

void main()
{
    fColor = texture(uTexture, vUV);
}