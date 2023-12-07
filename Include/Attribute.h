#pragma once

#include <Shader.h>
#include <vector>

struct Attribute {
    const char* name;
    int count;
    int type;
    int bytesize;

    static int configure(std::vector<Attribute> attributes, Shader* shader, int VBO, int VAO);
};

// IMPLEMENTATION ======================================================================================================
#include <GL/glew.h>

int Attribute::configure(std::vector<Attribute> attributes, Shader* shader, int VBO, int VAO) {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    int stride = 0;
    int componentCount = 0;
    for (Attribute attribute : attributes) {
        stride += attribute.count * attribute.bytesize;
        componentCount += attribute.count;
    }

    int offset = 0;
    for (int i = 0; i < attributes.size(); i++) {
        Attribute attribute = attributes[i];
        int location = shader->getAttributeLocation(attribute.name);
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, attribute.count, attribute.type, GL_FALSE, stride, (void*) offset);
        offset += attribute.count * attribute.bytesize;
    }

    return componentCount;
}