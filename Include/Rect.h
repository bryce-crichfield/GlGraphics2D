#pragma once

#include <Shader.h>
#include <vector>
#include <memory>


class Rect 
{
public:
    Rect();
    ~Rect();
    void rect(float x, float y, float width, float height);
    void draw(float* view);
    void setRoundness(float roundness);
    void setColor(float r, float g, float b, float a);
    void setThickness(float thickness);
  private:
    std::unique_ptr<Shader> mShader;
    unsigned int mVAO, mVBO;
    std::vector<float> mVertices;
    float mRoundness;
    float mR, mG, mB, mA;
    float mThickness;
};

// IMPLEMENTATION ======================================================================================================
#include <FileUtils.h>
#include <Attribute.h>

#include <GL/glew.h>

Rect::Rect()
{
    std::string vertexShader   = FileUtils::readTextFile("../Assets/Shaders/Rect/Rect.vert");
    std::string geometryShader = FileUtils::readTextFile("../Assets/Shaders/Rect/Rect.geom");
    std::string fragmentShader = FileUtils::readTextFile("../Assets/Shaders/Rect/Rect.frag");
    Shader* shader = Shader::loadFromSource(vertexShader.c_str(), geometryShader.c_str(), fragmentShader.c_str());
    mShader = std::unique_ptr<Shader>(shader);

    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    std::vector<Attribute> attributes = {
        {"aPos", 2, GL_FLOAT, sizeof(float)},
        {"aSize", 2, GL_FLOAT, sizeof(float)},
        {"aRoundness", 1, GL_FLOAT, sizeof(float)},
        {"aColor", 4, GL_FLOAT, sizeof(float)},
        {"aThickness", 1, GL_FLOAT, sizeof(float)}
    };

    Attribute::configure(attributes, mShader.get(), mVBO, mVAO);
}

Rect::~Rect()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
}

void Rect::rect(float x, float y, float width, float height)
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
}

void Rect::draw(float* view)
{
    mShader->use();
    mShader->setMat4("uView", view);

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(float), &mVertices[0], GL_STATIC_DRAW);
    glDrawArrays(GL_POINTS, 0, mVertices.size() / 10);
    glBindVertexArray(0);

    mVertices.clear();
}