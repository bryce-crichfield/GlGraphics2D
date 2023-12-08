#pragma once

#include <Shader.h>
#include <memory>
#include <vector>

class Graphics
{
  public:
    Graphics();
    ~Graphics();

    void drawLine(float x1, float y1, float x2, float y2);
    void drawRect(float x, float y, float width, float height);
    void drawEllipse(float x, float y, float width, float height);
    void drawCircle(float x, float y, float radius);
    void drawArc(float x, float y, float width, float height, float startAngle, float endAngle);
    void fillRect(float x, float y, float width, float height);
    void fillEllipse(float x, float y, float width, float height);
    void fillCircle(float x, float y, float radius);

    void setViewport(int width, int height);
    void flush();

    void setRoundness(float roundness);
    void setColor(float r, float g, float b, float a = 1.0f);
    void setThickness(float thickness);

  private:
    enum class PolygonType
    {
        LINE = 0,
        FILLED_RECT = 1,
        OUTLINE_RECT = 2,
        FILLED_CIRCLE = 3,
        OUTLINE_CIRCLE = 4,
        ARC = 5
    };

    std::unique_ptr<Shader> mShader;
    unsigned int mVAO, mVBO;
    std::vector<float> mVertices;
    float mRoundness = 1.0f;
    float mR, mG, mB, mA = 1.0f;
    float mThickness = 1.0f;
    float mZLayer = 1;
    float mView[16];

    int mComponentCount;

    void pushAttributes(float a, float b, float c, float d, PolygonType polygonType, float startAngle = 0,
                        float endAngle = 0);
};

// IMPLEMENTATION ======================================================================================================
#include <Attribute.h>

#include <GL/glew.h>
#include <glm/ext.hpp>

const char* vertexShaderSource = R"(
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
)";

const char* geometryShaderSource = R"(
    #version 400 core

    layout(points) in;
    layout(triangle_strip, max_vertices = 128) out;

    in vec2 vDimension[];
    in vec4 vColor[];
    in float vRoundness[];
    in float vThickness[];
    in float vPolygonType[];
    in float vZLayer[];
    in vec2 vArcAngles[];

    out vec4 fColor;
    out vec2 fTexCoord;

    uniform mat4 uView;

    const float PI = 3.1415926535897932384626433832795;

    const int POLYGON_TYPE_LINE = 0;
    const int POLYGON_TYPE_FILLED_RECT = 1;
    const int POLYGON_TYPE_OUTLINE_RECT = 2;
    const int POLYGON_TYPE_FILLED_ELLIPSE = 3;
    const int POLYGON_TYPE_OUTLINE_ELLIPSE = 4;
    const int POLYGON_TYPE_ARC = 5;

    void emit(vec2 pos)
    {
        gl_Position = uView * vec4(pos, vZLayer[0], 1.0);
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
        float w = vDimension[0].x / 2.0;
        float h = vDimension[0].y / 2.0;
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
        float w = vDimension[0].x / 2.0;
        float h = vDimension[0].y / 2.0;
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

    void doLine()
    {
        // Lines are described by their center point, their direction and their length
        // Direction is aSize
        // Length is aRoundness

        vec2 center = gl_in[0].gl_Position.xy;
        vec2 direction = vDimension[0];
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
    }

    void doFilledEllipse()
    {
        // the ellipse is described by its center point, and its size
        vec2 center = gl_in[0].gl_Position.xy;
        float w = vDimension[0].x / 2.0;
        float h = vDimension[0].y / 2.0;

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
        float w = vDimension[0].x / 2.0;
        float h = vDimension[0].y / 2.0;

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

    void doArc() 
    {
        // The arc is described by its top left corner, its bounding box and its angle
        vec2 origin = gl_in[0].gl_Position.xy;
        float width = vDimension[0].x;
        float height = vDimension[0].y;
        float startAngle = radians(vArcAngles[0].x);
        startAngle = clamp(startAngle, 0.0, 2.0 * PI);
        float endAngle = radians(vArcAngles[0].y);
        endAngle = clamp(endAngle, 0.0, 2.0 * PI);
        float thickness = vThickness[0];

        int segmentCount = 60;

        float angle = startAngle;

        float minXRadius = width / 2.0 - thickness / 2.0;
        float maxXRadius = width / 2.0 + thickness / 2.0;
        float minYRadius = height / 2.0 - thickness / 2.0;
        float maxYRadius = height / 2.0 + thickness / 2.0;


        for (int i = 0; i <= segmentCount; i++)
        {
            float sinAngle = sin(angle);
            float cosAngle = cos(angle);

            float x1 = cosAngle * minXRadius;
            x1 -= thickness / 2.0;
            float y1 = sinAngle * minYRadius;
            y1 -= thickness / 2.0;
            float x2 = cosAngle * maxXRadius;
            x2 -= thickness / 2.0;
            float y2 = sinAngle * maxYRadius;
            y2 -= thickness / 2.0;

            // the arc should be centered in the bounding box
            x1 += width / 2.0;
            y1 += height / 2.0;
            x2 += width / 2.0;
            y2 += height / 2.0;

            emit(origin + vec2(x1, y1));
            emit(origin + vec2(x2, y2));
            angle += (endAngle - startAngle) / segmentCount;
        }

        EndPrimitive();
    };

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
                doLine();
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
            case POLYGON_TYPE_ARC:
                doArc();
                break;
            default:
                break;
        }
    }
)";

const char* fragmentShaderSource = R"(
    #version 400 core

    in vec4 fColor;

    out vec4 FragColor;

    void main() {
        FragColor = fColor;
    }
)";

Graphics::Graphics()
{
    Shader* shader = Shader::loadFromSource(vertexShaderSource, geometryShaderSource, fragmentShaderSource);
    mShader = std::unique_ptr<Shader>(shader);

    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    std::vector<Attribute> attributes = {
        {"aPos", 2, GL_FLOAT, sizeof(float)},       {"aDimension", 2, GL_FLOAT, sizeof(float)},
        {"aRoundness", 1, GL_FLOAT, sizeof(float)}, {"aColor", 4, GL_FLOAT, sizeof(float)},
        {"aThickness", 1, GL_FLOAT, sizeof(float)}, {"aPolygonType", 1, GL_FLOAT, sizeof(float)},
        {"aZLayer", 1, GL_FLOAT, sizeof(float)},    {"aArcAngles", 2, GL_FLOAT, sizeof(float)},
    };

    mComponentCount = Attribute::configure(attributes, mShader.get(), mVBO, mVAO);
}

Graphics::~Graphics()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
}

void Graphics::pushAttributes(float a, float b, float c, float d, PolygonType polygonType, float startAngle,
                             float endAngle)
{
    mVertices.push_back(a);
    mVertices.push_back(b);
    mVertices.push_back(c);
    mVertices.push_back(d);
    mVertices.push_back(mRoundness);
    mVertices.push_back(mR);
    mVertices.push_back(mG);
    mVertices.push_back(mB);
    mVertices.push_back(mA);
    mVertices.push_back(mThickness);
    mVertices.push_back((float) polygonType);
    mVertices.push_back(mZLayer++);
    mVertices.push_back(startAngle);
    mVertices.push_back(endAngle);
}

void Graphics::drawLine(float x1, float y1, float x2, float y2)
{
    // Lines are described by their center point, their direction and their length
    // Direction is aSize
    // Length is aRoundness
    glm::vec2 direction = glm::vec2(x2 - x1, y2 - y1);
    float length = glm::length(direction);
    pushAttributes(x1, y1, direction.x, direction.y, PolygonType::LINE);
};

void Graphics::drawRect(float x, float y, float width, float height)
{
    x = x + (width / 2.0f);
    y = y + (height / 2.0f);
    pushAttributes(x, y, width, height, PolygonType::OUTLINE_RECT);
}

void Graphics::drawArc(float x, float y, float width, float height, float startAngle, float endAngle)
{
    pushAttributes(x, y, width, height, PolygonType::ARC, startAngle, endAngle);
}

void Graphics::drawCircle(float x, float y, float radius)
{
    drawEllipse(x, y, radius, radius);
}

void Graphics::fillCircle(float x, float y, float radius)
{
    fillEllipse(x, y, radius, radius);
}

void Graphics::fillRect(float x, float y, float width, float height)
{
    x = x + (width / 2.0f);
    y = y + (height / 2.0f);
    pushAttributes(x, y, width, height, PolygonType::FILLED_RECT);
}

void Graphics::drawEllipse(float x, float y, float width, float height)
{
    // the ellipse is described by its center point, its width and its height
    x = x + (width / 2.0f);
    y = y + (height / 2.0f);
    pushAttributes(x, y, width, height, PolygonType::OUTLINE_CIRCLE);
}

void Graphics::fillEllipse(float x, float y, float width, float height)
{
    // the ellipse is described by its center point, its width and its height
    x = x + (width / 2.0f);
    y = y + (height / 2.0f);
    pushAttributes(x, y, width, height, PolygonType::FILLED_CIRCLE);
}

void Graphics::setViewport(int width, int height)
{
    glm::mat4 viewMatrix = glm::ortho(0.0f, (float) width, (float) height, 0.0f, -1000000.0f, 0.01f);
    memcpy(mView, glm::value_ptr(viewMatrix), 16 * sizeof(float));
}

void Graphics::flush()
{
    mShader->use();
    mShader->setMat4("uView", mView);

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(float), &mVertices[0], GL_STATIC_DRAW);
    glDrawArrays(GL_POINTS, 0, mVertices.size() / mComponentCount);
    glBindVertexArray(0);

    mVertices.clear();
    mZLayer = 1;
}

void Graphics::setRoundness(float roundness)
{
    mRoundness = roundness;
}

void Graphics::setColor(float r, float g, float b, float a)
{
    mR = r;
    mG = g;
    mB = b;
    mA = a;
}

void Graphics::setThickness(float thickness)
{
    mThickness = thickness;
}