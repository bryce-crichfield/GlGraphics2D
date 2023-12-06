#version 400 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aSize;
layout (location = 2) in vec4 aColor;
layout (location = 3) in float aRoundness;
layout (location = 4) in float aThickness;
layout (location = 5) in float aPolygonType;

out vec2 vSize;
out vec4 vColor;
out float vRoundness;
out float vThickness;
out float vPolygonType;

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    vSize = aSize;
    vColor = aColor;
    vRoundness = aRoundness;
    vThickness = aThickness;
    vPolygonType = aPolygonType;
}