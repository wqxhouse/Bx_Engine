#pragma once

#include "ShaderCompiler.h"

class Shader
{
public:
    Shader();

    ~Shader();

    void setShaderFiles(
        char* vertexShaderFile,
        char* fragmentShaderFile);

    BOOL linkProgram();

    void useProgram();
    void finishProgram();

    inline GLuint GetShaderProgram() const { return m_shaderProgram; }

private:
    GLuint m_shaderProgram;

    ShaderCompiler m_shaderCompiler;

    char* m_vertexShaderFile;
    char* m_fragmentShaderFile;

    char** pGeometryShaderFile;
};