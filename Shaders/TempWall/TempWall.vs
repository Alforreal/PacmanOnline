#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 Bmodel;
uniform mat4 Bview;
uniform mat4 Bprojection;
uniform vec4 BColor;

out vec4 Color;
void main()
{
    gl_Position = Bprojection * Bview * Bmodel * vec4(aPos, 1.0f);
    Color = BColor;
}