#pragma once

#include <Shader.h>
#include <memory>
#include <vector>

class Line
{
  public:
    Line();
    ~Line();

    void line(float x1, float y1, float x2, float y2);
    void render(float* view);
    void setColor(float r, float g, float b);
    void setThickness(float thickness);

  private:
    std::unique_ptr<Shader> shader;
    unsigned int VAO, VBO;
    std::vector<float> vertices;
    float r, g, b;
    float thickness = 15.0f;
};

// IMPLEMENTATION ======================================================================================================
#include <Attribute.h>
#include <FileUtils.h>
#include <GL/glew.h>

Line::Line()
{
    std::string vert = FileUtils::readTextFile("../Assets/Shaders/Line/Line.vert");
    std::string geom = FileUtils::readTextFile("../Assets/Shaders/Line/Line.geom");
    std::string frag = FileUtils::readTextFile("../Assets/Shaders/Line/Line.frag");
    shader = std::unique_ptr<Shader>(Shader::loadFromSource(vert.c_str(), geom.c_str(), frag.c_str()));

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    std::vector<Attribute> attributes = {{"aPos", 2, GL_FLOAT, sizeof(float)},
                                         {"aColor", 3, GL_FLOAT, sizeof(float)},
                                         {"aThickness", 1, GL_FLOAT, sizeof(float)}};

    Attribute::configure(attributes, shader.get(), VBO, VAO);
}

Line::~Line()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Line::line(float x1, float y1, float x2, float y2)
{
    vertices.push_back(x1);
    vertices.push_back(y1);
    vertices.push_back(r);
    vertices.push_back(g);
    vertices.push_back(b);
    vertices.push_back(thickness);

    vertices.push_back(x2);
    vertices.push_back(y2);
    vertices.push_back(r);
    vertices.push_back(g);
    vertices.push_back(b);
    vertices.push_back(thickness);
}

void Line::render(float* view)
{
    shader->use();
    shader->setMat4("view", view);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_LINES, 0, vertices.size() / 6);
    vertices.clear();
}

void Line::setColor(float r, float g, float b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}

void Line::setThickness(float thickness)
{
    this->thickness = thickness;
}