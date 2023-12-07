#version 400 core

layout(points) in;
layout(triangle_strip, max_vertices = 128) out;

in vec2 vSize[];
in vec4 vColor[];
in float vRoundness[];
in float vThickness[];
in float vPolygonType[];
in float vZCounter[];

out vec4 fColor;

uniform mat4 uView;

const float PI = 3.1415926535897932384626433832795;

const int POLYGON_TYPE_ROUNDED_LINE = -1;
const int POLYGON_TYPE_LINE = 0;
const int POLYGON_TYPE_FILLED_RECT = 1;
const int POLYGON_TYPE_OUTLINE_RECT = 2;
const int POLYGON_TYPE_FILLED_ELLIPSE = 3;
const int POLYGON_TYPE_OUTLINE_ELLIPSE = 4;

void emit(vec2 pos)
{
    gl_Position = uView * vec4(pos, vZCounter[0], 1.0);
    EmitVertex();
}

void filledCorner(vec2 origin, float startAngle, float endAngle, float radius)
{
    int vertexCount = 12;

    float angle = startAngle;

    for (int i = 0; i <= vertexCount; i++)
    {
        float x = cos(angle) * radius;
        float y = sin(angle) * radius;

        emit(origin + vec2(x, y));
        emit(origin);
        angle += (endAngle - startAngle) / vertexCount;
    }
    EndPrimitive();
}

void outlineCorner(vec2 origin, float startAngle, float endAngle, float radius, float thickness)
{
    int segmentCount = 8;

    // Draw Arc

    float angle = startAngle;

    // clamp thickness so its not bigger than radius (causes weird artifacts)
    thickness = min(thickness, radius);

    float minRadius = radius - thickness / 2.0;
    minRadius -= (thickness / 2);
    float maxRadius = radius + thickness / 2.0;
    maxRadius -= (thickness / 2);

    for (int i = 0; i <= segmentCount; i++)
    {
        float sinAngle = sin(angle);
        float cosAngle = cos(angle);

        float x1 = cosAngle * minRadius;
        float y1 = sinAngle * minRadius;
        float x2 = cosAngle * maxRadius;
        float y2 = sinAngle * maxRadius;

        emit(origin + vec2(x1, y1));
        emit(origin + vec2(x2, y2));
        angle += (endAngle - startAngle) / segmentCount;
    }

    EndPrimitive();
}

void doFilledRect()
{
    // Filled rects are described by their center point, their size and their roundness
    vec2 center = gl_in[0].gl_Position.xy;
    float w = vSize[0].x / 2.0;
    float h = vSize[0].y / 2.0;
    float r = vRoundness[0];

    // CENTER (TL, TR, BL, BR)
    emit(center + vec2(-w + r, -h + r));
    emit(center + vec2(w - r, -h + r));
    emit(center + vec2(-w + r, h - r));
    emit(center + vec2(w - r, h - r));
    EndPrimitive();

    // LEFT (TL, TR, BL, BR)
    emit(center + vec2(-w, -h + r));
    emit(center + vec2(-w + r, -h + r));
    emit(center + vec2(-w, h - r));
    emit(center + vec2(-w + r, h - r));
    EndPrimitive();
    // RIGHT (TL, TR, BL, BR)
    emit(center + vec2(w - r, -h + r));
    emit(center + vec2(w, -h + r));
    emit(center + vec2(w - r, h - r));
    emit(center + vec2(w, h - r));
    EndPrimitive();
    // TOP (TL, TR, BL, BR)
    emit(center + vec2(-w + r, -h));
    emit(center + vec2(w - r, -h));
    emit(center + vec2(-w + r, -h + r));
    emit(center + vec2(w - r, -h + r));
    EndPrimitive();
    // BOTTOM (TL, TR, BL, BR)
    emit(center + vec2(-w + r, h - r));
    emit(center + vec2(w - r, h - r));
    emit(center + vec2(-w + r, h));
    emit(center + vec2(w - r, h));
    EndPrimitive();

    if (r != 0.0)
    {
        return;
    }

    // TOP LEFT
    filledCorner(center + vec2(-w + r, -h + r), PI, PI * 1.5, r);
    // TOP RIGHT
    filledCorner(center + vec2(w - r, -h + r), PI * 1.5, PI * 2.0, r);
    // BOTTOM LEFT
    filledCorner(center + vec2(-w + r, h - r), PI * 0.5, PI, r);
    // BOTTOM RIGHT
    filledCorner(center + vec2(w - r, h - r), 0.0, PI * 0.5, r);
}

void doOutlineRect()
{
    // Outline rects are described by their center point, their size, their roundness and their thickness
    vec2 center = gl_in[0].gl_Position.xy;
    float w = vSize[0].x / 2.0;
    float h = vSize[0].y / 2.0;
    float r = vRoundness[0];
    float t = vThickness[0];

    // LEFT (tl, tr, bl, br)
    emit(center + vec2(-w, -h + r));
    emit(center + vec2(-w + t, -h + r));
    emit(center + vec2(-w, h - r));
    emit(center + vec2(-w + t, h - r));
    EndPrimitive();
    // TOP (tl, tr, bl, br)
    emit(center + vec2(-w + r, -h));
    emit(center + vec2(w - r, -h));
    emit(center + vec2(-w + r, -h + t));
    emit(center + vec2(w - r, -h + t));
    EndPrimitive();
    // RIGHT (tl, tr, bl, br)
    emit(center + vec2(w - t, -h + r));
    emit(center + vec2(w, -h + r));
    emit(center + vec2(w - t, h - r));
    emit(center + vec2(w, h - r));
    EndPrimitive();
    // BOTTOM (tl, tr, bl, br)
    emit(center + vec2(-w + r, h - t));
    emit(center + vec2(w - r, h - t));
    emit(center + vec2(-w + r, h));
    emit(center + vec2(w - r, h));
    EndPrimitive();

    // TOP LEFT
    outlineCorner(center + vec2(-w + r, -h + r), PI, PI * 1.5, r, t);
    // TOP RIGHT
    outlineCorner(center + vec2(w - r, -h + r), PI * 1.5, PI * 2.0, r, t);
    // BOTTOM LEFT
    outlineCorner(center + vec2(-w + r, h - r), PI * 0.5, PI, r, t);
    // BOTTOM RIGHT
    outlineCorner(center + vec2(w - r, h - r), 0.0, PI * 0.5, r, t);
}

void drawCaps(vec2 center, float thickness, float angleOffset)
{
    int CIRCLE_VERT_COUNT = 6;
    float radius = thickness / 2.0f;

    float angle;
    for (int i = 0; i <= CIRCLE_VERT_COUNT; i++)
    {
        angle = (i * PI) / float(CIRCLE_VERT_COUNT);
        angle += angleOffset;
        float x = cos(angle) * radius;
        float y = sin(angle) * radius;

        emit(center + vec2(x, y));
        emit(center);
    }

    EndPrimitive();
}

void doLine(bool rounded)
{
    // Lines are described by their center point, their direction and their length
    // Direction is aSize
    // Length is aRoundness

    vec2 center = gl_in[0].gl_Position.xy;
    vec2 direction = vSize[0];
    float length = vRoundness[0];
    float thickness = vThickness[0];

    vec2 normal = normalize(vec2(-direction.y, direction.x));
    vec2 halfNormal = normal * thickness / 2.0;

    vec2 p1 = center + halfNormal;
    vec2 p2 = center - halfNormal;
    vec2 p3 = center + direction + halfNormal;
    vec2 p4 = center + direction - halfNormal;

    emit(p1);
    emit(p2);
    emit(p3);
    emit(p4);
    EndPrimitive();

    if (rounded)
    {
        vec2 dir = normalize(direction);

        vec2 startCapCenter = center - normal;
        float startCapAngleOffset = atan(dir.y, dir.x) + (PI / 2.0);
        drawCaps(startCapCenter, vThickness[0], startCapAngleOffset);

        vec2 endCapCenter = center + normal + direction;
        float endCapAngleOffset = atan(dir.y, dir.x) - (PI / 2.0);
        drawCaps(endCapCenter, vThickness[0], endCapAngleOffset);
    }
}

void doFilledEllipse()
{
    // the ellipse is described by its center point, and its size
    vec2 center = gl_in[0].gl_Position.xy;
    float w = vSize[0].x / 2.0;
    float h = vSize[0].y / 2.0;

    int vertexCount = 56;

    float angle = 0.0;

    for (int i = 0; i <= vertexCount; i++)
    {
        float x = cos(angle) * w;
        float y = sin(angle) * h;

        emit(center + vec2(x, y));
        emit(center);
        angle += (2.0 * PI) / vertexCount;
    }

    EndPrimitive();
}

void doOutlineEllipse()
{
    // the ellipse is described by its center point, its size, and its thickness
    vec2 center = gl_in[0].gl_Position.xy;
    float w = vSize[0].x / 2.0;
    float h = vSize[0].y / 2.0;

    int segmentCount = 60;

    float angle = 0.0;

    float minXRadius = w - vThickness[0] / 2.0;
    float maxXRadius = w + vThickness[0] / 2.0;
    float minYRadius = h - vThickness[0] / 2.0;
    float maxYRadius = h + vThickness[0] / 2.0;

    for (int i = 0; i <= segmentCount; i++)
    {
        float sinAngle = sin(angle);
        float cosAngle = cos(angle);

        float x1 = cosAngle * minXRadius;
        float y1 = sinAngle * minYRadius;
        float x2 = cosAngle * maxXRadius;
        float y2 = sinAngle * maxYRadius;

        emit(center + vec2(x1, y1));
        emit(center + vec2(x2, y2));

        angle += (2.0 * PI) / segmentCount;
    }

    EndPrimitive();
}

void main()
{
    int polygonType = int(vPolygonType[0]);
    fColor = vColor[0];

    // Measured the amortized cost of branching vs. not branching, and 
    // it was about the same so I'm just going to branch, and save myself 
    // from extra client side code.
    switch (polygonType)
    {
        case POLYGON_TYPE_LINE:
            doLine(false);
            break;
        case POLYGON_TYPE_ROUNDED_LINE:
            doLine(true);
            break;
        case POLYGON_TYPE_FILLED_RECT:
            doFilledRect();
            break;
        case POLYGON_TYPE_OUTLINE_RECT:
            doOutlineRect();
            break;
        case POLYGON_TYPE_FILLED_ELLIPSE:
            doFilledEllipse();
            break;
        case POLYGON_TYPE_OUTLINE_ELLIPSE:
            doOutlineEllipse();
            break;
        default:
            break;
    }
}