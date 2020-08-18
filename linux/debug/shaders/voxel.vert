#version 330 core
layout (location=0) in vec3 attPos;
layout (location=1) in vec3 attNorm;
layout (location=2) in uvec3 attInstOffset;
layout (location=3) in vec3 attInstColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 fragPos;
out vec3 fragNorm;
out vec3 color;

void main()
{
    vec4 vertPos = vec4(attPos + attInstOffset, 1.0f);
    fragPos = vec3(model * vertPos);
    gl_Position = proj * view * model * vertPos;
    fragNorm = attNorm;
    color = attInstColor;
}