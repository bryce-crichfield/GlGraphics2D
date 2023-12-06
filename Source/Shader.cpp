#include <Shader.h>

#include <GL/glew.h>
#include <iostream>

Shader::Shader(unsigned int programId)
{
    mProgramId = programId;
}

Shader::~Shader()
{
    glDeleteProgram(mProgramId);
}

void Shader::use() const
{
    glUseProgram(mProgramId);
}

unsigned int Shader::getProgramId() const
{
    return mProgramId;
}

unsigned int Shader::getAttributeLocation(const char* name) const
{
    unsigned int location = glGetAttribLocation(mProgramId, name);
    if (location == -1)
    {
        std::cout << "Shader::getAttributeLocation: " << name << " not found" << std::endl;
    }

    return location;
}

void Shader::setBool(const char* name, bool value) const
{
    unsigned int location = glGetUniformLocation(mProgramId, name);
    if (location == -1)
    {
        std::cout << "Shader::setBool: " << name << " not found" << std::endl;
        return;
    }

    glUniform1i(location, (int) value);
}

void Shader::setInt(const char* name, int value) const
{
    unsigned int location = glGetUniformLocation(mProgramId, name);
    if (location == -1)
    {
        std::cout << "Shader::setInt: " << name << " not found" << std::endl;
        return;
    }

    glUniform1i(location, value);
}

void Shader::setFloat(const char* name, float value) const
{
    unsigned int location = glGetUniformLocation(mProgramId, name);
    if (location == -1)
    {
        std::cout << "Shader::setFloat: " << name << " not found" << std::endl;
        return;
    }

    glUniform1f(location, value);
}

void Shader::setVec2(const char* name, float x, float y) const
{
    unsigned int location = glGetUniformLocation(mProgramId, name);
    if (location == -1)
    {
        std::cout << "Shader::setVec2: " << name << " not found" << std::endl;
        return;
    }

    glUniform2f(location, x, y);
}

void Shader::setVec3(const char* name, float x, float y, float z) const
{
    unsigned int location = glGetUniformLocation(mProgramId, name);
    if (location == -1)
    {
        std::cout << "Shader::setVec3: " << name << " not found" << std::endl;
        return;
    }

    glUniform3f(location, x, y, z);
}

void Shader::setVec4(const char* name, float x, float y, float z, float w) const
{
    unsigned int location = glGetUniformLocation(mProgramId, name);
    if (location == -1)
    {
        std::cout << "Shader::setVec4: " << name << " not found" << std::endl;
        return;
    }

    glUniform4f(location, x, y, z, w);
}

void Shader::setMat4(const char* name, float* mat) const
{
    unsigned int location = glGetUniformLocation(mProgramId, name);
    if (location == -1)
    {
        std::cout << "Shader::setMat4: " << name << " not found" << std::endl;
        return;
    }

    glUniformMatrix4fv(location, 1, GL_FALSE, mat);
}

Shader* Shader::loadFromSource(const char* vertShader, const char* geomShader, const char* fragShader)
{
    int success;
    char infoLog[512];

    GLuint vertShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShaderId, 1, &vertShader, NULL);
    glCompileShader(vertShaderId);
    glGetShaderiv(vertShaderId, GL_COMPILE_STATUS, &success);
    if (! success)
    {
        glGetShaderInfoLog(vertShaderId, 512, NULL, infoLog);
        std::cout << "Shader::loadFromSource: Vertex shader compilation failed\n" << infoLog << std::endl;
        return nullptr;
    }

    GLuint geomShaderId = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geomShaderId, 1, &geomShader, NULL);
    glCompileShader(geomShaderId);
    glGetShaderiv(geomShaderId, GL_COMPILE_STATUS, &success);
    if (! success)
    {
        glGetShaderInfoLog(geomShaderId, 512, NULL, infoLog);
        std::cout << "Shader::loadFromSource: Geometry shader compilation failed\n" << infoLog << std::endl;
        return nullptr;
    }

    GLuint fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShaderId, 1, &fragShader, NULL);
    glCompileShader(fragShaderId);
    glGetShaderiv(fragShaderId, GL_COMPILE_STATUS, &success);
    if (! success)
    {
        glGetShaderInfoLog(fragShaderId, 512, NULL, infoLog);
        std::cout << "Shader::loadFromSource: Fragment shader compilation failed\n" << infoLog << std::endl;
        return nullptr;
    }

    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertShaderId);
    glAttachShader(programId, geomShaderId);
    glAttachShader(programId, fragShaderId);
    glLinkProgram(programId);
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (! success)
    {
        glGetProgramInfoLog(programId, 512, NULL, infoLog);
        std::cout << "Shader::loadFromSource: Shader program linking failed\n" << infoLog << std::endl;
        return nullptr;
    }

    glDeleteShader(vertShaderId);
    glDeleteShader(geomShaderId);
    glDeleteShader(fragShaderId);

    return new Shader(programId);
}