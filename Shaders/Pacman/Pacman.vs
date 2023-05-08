#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 Pmodel;
uniform mat4 Pview;
uniform mat4 Pprojection;
uniform vec4 InputColor;

out vec4 Color;
void main()
{
    gl_Position = Pprojection * Pview * Pmodel * vec4(aPos, 1.0f);
    Color = InputColor;
}