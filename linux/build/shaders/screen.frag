#version 330 core
in vec3 vertPos;
in vec2 texCoord;

uniform float scaleFactor;
uniform sampler2D texData;

out vec4 fragColor;

void main()
{
    fragColor = texture(texData, texCoord);
}
