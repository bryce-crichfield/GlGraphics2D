#version 400 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in float aThickness;

out vec3 vColor;
out float vThickness;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    vColor = aColor;
    vThickness = aThickness;
}