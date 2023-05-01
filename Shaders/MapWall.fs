#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 Wmodel;
uniform mat4 Wview;
uniform mat4 Wprojection;
uniform vec4 WColor;

out vec4 Color;
void main()
{
    gl_Position = Wprojection * Wview * Wmodel * vec4(aPos, 1.0f);
    Color = WColor;
}