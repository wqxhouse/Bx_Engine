#include "Shader.h"

Shader::Shader()
{
}

Shader::~Shader()
{
    glDeleteProgram(m_shaderProgram);
}

void Shader::setShaderFiles(
    char* const vertexShaderFile,
    char* const fragmentShaderFile)
{
    m_vertexShaderFile   = vertexShaderFile;
    m_fragmentShaderFile = fragmentShaderFile;
}

BOOL Shader::linkProgram()
{
    BOOL result = m_shaderCompiler.compileShader(
            m_vertexShaderFile, m_fragmentShaderFile, &m_shaderProgram);

    return result;
}

GLuint Shader::useProgram()
{
    glUseProgram(m_shaderProgram);

    return m_shaderProgram;
}

void Shader::finishProgram()
{
    glUseProgram(0);
}

void Shader::assertErrors(const char* const str)
{
    printf(str);
    assert(FALSE);
}
