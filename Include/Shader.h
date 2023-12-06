#pragma once

class Shader
{
  public:
    Shader() = default;
    Shader(unsigned int programId);
    Shader(const Shader& other);
    Shader(Shader&& other);
    Shader operator=(const Shader& other);

    ~Shader();

    void use() const;

    unsigned int getProgramId() const;
    unsigned int getAttributeLocation(const char* name) const;

    void setBool(const char* name, bool value) const;
    void setInt(const char* name, int value) const;
    void setFloat(const char* name, float value) const;
    void setVec2(const char* name, float x, float y) const;
    void setVec3(const char* name, float x, float y, float z) const;
    void setVec4(const char* name, float x, float y, float z, float w) const;
    void setMat4(const char* name, float* mat) const;

    static Shader* loadFromSource(const char* vertShader, const char* geomShader, const char* fragShader);

  private:
    unsigned int mProgramId;
};