#version 400 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aDimension;
layout (location = 2) in vec4 aColor;
layout (location = 3) in float aRoundness;
layout (location = 4) in float aThickness;
layout (location = 5) in float aPolygonType;
layout (location = 6) in float aZLayer;
layout (location = 7) in vec2 aArcAngles;

out vec2 vDimension;
out vec4 vColor;
out float vRoundness;
out float vThickness;
out float vPolygonType;
out float vZLayer;
out vec2 vArcAngles;
out vec4 vEndColor;

void main() {
    gl_Position = vec4(aPos, 0, 1.0);
    vDimension = aDimension;
    vColor = aColor;
    vRoundness = aRoundness;
    vThickness = aThickness;
    vPolygonType = aPolygonType;
    vZLayer = aZLayer;
    vArcAngles = aArcAngles;
}