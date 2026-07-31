#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

out vec2 vUV;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProject; 

void main()
{
    gl_Position = uProject * uView * uModel * vec4(position, 1.0);
    vUV = uv;
}