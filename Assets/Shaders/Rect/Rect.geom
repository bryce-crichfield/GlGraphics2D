#version 400 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec2 vSize[];
in vec4 vColor[];
in float vRoundness[];
in float vThickness[];

out vec4 fColor;

uniform mat4 uView;

void emit(vec2 pos) {
    gl_Position = uView * vec4(pos, 0.0, 1.0);
    EmitVertex();
}

void main() {
    fColor = vColor[0];

    vec2 center = gl_in[0].gl_Position.xy;
    vec2 size = vSize[0];

    vec2 p0 = center + vec2(-size.x, -size.y) * 0.5;
    vec2 p1 = center + vec2(-size.x, size.y) * 0.5;
    vec2 p2 = center + vec2(size.x, -size.y) * 0.5;
    vec2 p3 = center + vec2(size.x, size.y) * 0.5;

    emit(p0);
    emit(p1);
    emit(p2);
    emit(p3);

    EndPrimitive();
}