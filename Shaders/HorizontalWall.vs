#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 Wmodel;
uniform mat4 Wview;
uniform mat4 Wprojection;

void main()
{
    gl_Position = Wprojection * Wview * Wmodel * vec4(aPos, 1.0f);
}