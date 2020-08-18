#version 330 core
layout (location=0) in vec3 attPos;
layout (location=1) in vec3 attColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 vertColor;

void main()
{
    gl_Position = proj * view * model * vec4(attPos, 1.0f);
    vertColor = attColor;
}