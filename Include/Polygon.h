#pragma once

#include <Shader.h>
#include <memory>
#include <vector>

class Polygon
{
  public:
    Polygon();
    ~Polygon();
    void drawLine(float x1, float y1, float x2, float y2, bool rounded = false);
    void drawRect(float x, float y, float width, float height);
    void drawEllipse(float x, float y, float width, float height);
    void drawCircle(float x, float y, float radius)
    {
        drawEllipse(x, y, radius, radius);
    }

    void drawPolyline(std::vector<glm::vec2> points)
    {
        for (int i = 0; i < points.size() - 1; i++)
        {
            drawLine(points[i].x, points[i].y, points[i + 1].x, points[i + 1].y);
        }
    }

    void fillRect(float x, float y, float width, float height);
    void fillEllipse(float x, float y, float width, float height);
    void fillCircle(float x, float y, float radius)
    {
        fillEllipse(x, y, radius, radius);
    }

    void draw(float* view);

    void setRoundness(float roundness)
    {
        mRoundness = roundness;
    }

    void setColor(float r, float g, float b, float a = 1.0f)
    {
        mR = r;
        mG = g;
        mB = b;
        mA = a;
    }

    void setThickness(float thickness)
    {
        mThickness = thickness;
    }

  private:
    enum class PolygonType
    {
        ROUNDED_LINE = -1,
        LINE = 0,
        FILLED_RECT = 1,
        OUTLINE_RECT = 2,
        FILLED_CIRCLE = 3,
        OUTLINE_CIRCLE = 4,
    };

    std::unique_ptr<Shader> mShader;
    unsigned int mVAO, mVBO;
    std::vector<float> mVertices;
    float mRoundness = 10.0f;
    float mR, mG, mB, mA = 1.0f;
    float mThickness = 1.0f;
};

// IMPLEMENTATION ======================================================================================================
#include <Attribute.h>
#include <FileUtils.h>

#include <GL/glew.h>
#include <glm/ext.hpp>

Polygon::Polygon()
{
    std::string vertexShader = FileUtils::readTextFile("../Assets/Shaders/Polygon/Polygon.vert");
    std::string geometryShader = FileUtils::readTextFile("../Assets/Shaders/Polygon/Polygon.geom");
    std::string fragmentShader = FileUtils::readTextFile("../Assets/Shaders/Polygon/Polygon.frag");
    Shader* shader = Shader::loadFromSource(vertexShader.c_str(), geometryShader.c_str(), fragmentShader.c_str());
    mShader = std::unique_ptr<Shader>(shader);

    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    std::vector<Attribute> attributes = {
        {"aPos", 2, GL_FLOAT, sizeof(float)},       {"aSize", 2, GL_FLOAT, sizeof(float)},
        {"aRoundness", 1, GL_FLOAT, sizeof(float)}, {"aColor", 4, GL_FLOAT, sizeof(float)},
        {"aThickness", 1, GL_FLOAT, sizeof(float)}, {"aPolygonType", 1, GL_FLOAT, sizeof(float)},
    };

    Attribute::configure(attributes, mShader.get(), mVBO, mVAO);
}

Polygon::~Polygon()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
}

void Polygon::drawLine(float x1, float y1, float x2, float y2, bool rounded)
{
    // Lines are described by their center point, their direction and their length
    // Direction is aSize
    // Length is aRoundness
    glm::vec2 direction = glm::vec2(x2 - x1, y2 - y1);
    float length = glm::length(direction);

    mVertices.push_back(x1);
    mVertices.push_back(y1);
    mVertices.push_back(direction.x);
    mVertices.push_back(direction.y);
    mVertices.push_back(length);
    mVertices.push_back(mR);
    mVertices.push_back(mG);
    mVertices.push_back(mB);
    mVertices.push_back(mA);
    mVertices.push_back(mThickness);

    float polygonType = rounded ? (float) PolygonType::ROUNDED_LINE : (float) PolygonType::LINE;
    mVertices.push_back(polygonType);
};

void Polygon::drawRect(float x, float y, float width, float height)
{
    x = x + (width / 2.0f);
    y = y + (height / 2.0f);

    mVertices.push_back(x);
    mVertices.push_back(y);
    mVertices.push_back(width);
    mVertices.push_back(height);
    mVertices.push_back(mRoundness);
    mVertices.push_back(mR);
    mVertices.push_back(mG);
    mVertices.push_back(mB);
    mVertices.push_back(mA);
    mVertices.push_back(mThickness);
    mVertices.push_back((float) PolygonType::OUTLINE_RECT);
}

void Polygon::fillRect(float x, float y, float width, float height)
{
    x = x + (width / 2.0f);
    y = y + (height / 2.0f);

    mVertices.push_back(x);
    mVertices.push_back(y);
    mVertices.push_back(width);
    mVertices.push_back(height);
    mVertices.push_back(mRoundness);
    mVertices.push_back(mR);
    mVertices.push_back(mG);
    mVertices.push_back(mB);
    mVertices.push_back(mA);
    mVertices.push_back(mThickness);
    mVertices.push_back((float) PolygonType::FILLED_RECT);
}

void Polygon::drawEllipse(float x, float y, float width, float height)
{
    // the ellipse is described by its center point, its width and its height
    x = x + (width / 2.0f);
    y = y + (height / 2.0f);

    mVertices.push_back(x);
    mVertices.push_back(y);
    mVertices.push_back(width);
    mVertices.push_back(height);
    mVertices.push_back(mRoundness);
    mVertices.push_back(mR);
    mVertices.push_back(mG);
    mVertices.push_back(mB);
    mVertices.push_back(mA);
    mVertices.push_back(mThickness);
    mVertices.push_back((float) PolygonType::OUTLINE_CIRCLE);
}

void Polygon::fillEllipse(float x, float y, float width, float height)
{
    // the ellipse is described by its center point, its width and its height
    x = x + (width / 2.0f);
    y = y + (height / 2.0f);

    mVertices.push_back(x);
    mVertices.push_back(y);
    mVertices.push_back(width);
    mVertices.push_back(height);
    mVertices.push_back(mRoundness);
    mVertices.push_back(mR);
    mVertices.push_back(mG);
    mVertices.push_back(mB);
    mVertices.push_back(mA);
    mVertices.push_back(mThickness);
    mVertices.push_back((float) PolygonType::FILLED_CIRCLE);
}

void Polygon::draw(float* view)
{
    mShader->use();
    mShader->setMat4("uView", view);

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(float), &mVertices[0], GL_STATIC_DRAW);
    glDrawArrays(GL_POINTS, 0, mVertices.size() / 11);
    glBindVertexArray(0);

    mVertices.clear();
}