#version 400 core
layout(lines) in;
layout(triangle_strip, max_vertices = 128) out;

uniform mat4 view;

in vec3 vColor[];
in float vThickness[];
out vec3 fColor;

const float PI = 3.14159265;
const int CIRCLE_VERT_COUNT = 16;

void drawLine(float thickness)
{
    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;
    vec2 dir = normalize(p1.xy - p0.xy);
    vec2 normal = vec2(-dir.y, dir.x);
    vec4 offset = vec4(normal * thickness, 0.0, 0.0);
    gl_Position = view * (p0 + offset);
    EmitVertex();
    gl_Position = view * (p1 + offset);
    EmitVertex();
    gl_Position = view * (p0 - offset);
    EmitVertex();
    gl_Position = view * (p1 - offset);
    EmitVertex();
    EndPrimitive();
}

void drawCaps(vec4 centerPoint, float thickness, float angleOffset)
{
    float radius = thickness;
    vec4 center = centerPoint;

    float angle;
    for (int i = 0; i <= CIRCLE_VERT_COUNT; i++)
    {
        angle = (i * PI) / float(CIRCLE_VERT_COUNT);
        angle += angleOffset;
        float x = cos(angle) * radius;
        float y = sin(angle) * radius;
        gl_Position = view * (center + vec4(x, y, 0.0, 0.0));
        EmitVertex();
        gl_Position = view * (center);
        EmitVertex();
    }

    EndPrimitive();
}

void main()
{
    fColor = vColor[0];
    drawLine(vThickness[0]);
    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;
    vec2 dir = normalize(p1.xy - p0.xy);
    float angleOffset;
    angleOffset = atan(dir.y, dir.x) + (PI / 2.0);
    drawCaps(gl_in[0].gl_Position, vThickness[0], angleOffset);
    angleOffset = atan(dir.y, dir.x) - (PI / 2.0);
    drawCaps(gl_in[1].gl_Position, vThickness[0], angleOffset);
}