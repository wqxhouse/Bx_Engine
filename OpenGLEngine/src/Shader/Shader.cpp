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
    BOOL linkResult = TRUE;

    int result = 
        m_shaderCompiler.compileShader(
            m_vertexShaderFile, m_fragmentShaderFile, &m_shaderProgram);

    if (result != 0)
    {
        linkResult = FALSE;
    }

    return linkResult;
}

void Shader::useProgram()
{
    glUseProgram(m_shaderProgram);
}

void Shader::finishProgram()
{
    glUseProgram(0);
}
