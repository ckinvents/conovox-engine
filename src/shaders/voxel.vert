#version 330 core
layout (location=0) in vec3 attPos;
layout (location=1) in vec3 attNorm;
layout (location=2) in uint attInstOffset;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform uvec3 dims;
uniform sampler3D vox;

out vec3 fragPos;
out vec3 fragNorm;
out vec3 color;

void main()
{
    vec3 offset;
    uint layerSize = dims.y * dims.z;
    offset.y = attInstOffset / layerSize;
    offset.z = (attInstOffset - offset.y * layerSize) / dims.z;
    offset.x = attInstOffset - offset.y * layerSize - offset.z * dims.z;
    vec4 vertPos = vec4(attPos + offset, 1.0f);
    fragPos = vec3(model * vertPos);
    gl_Position = proj * view * model * vertPos;
    fragNorm = attNorm;
    color = attInstColor;
}