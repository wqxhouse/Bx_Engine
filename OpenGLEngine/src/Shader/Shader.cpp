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
    char* const fragmentShaderFile,
    char* const geometryShaderFile,
    char* const tesShaderFile,
    char* const tcsShaderFile)
{
    m_vertexShaderFile   = vertexShaderFile;
    m_fragmentShaderFile = fragmentShaderFile;
    m_geometryShaderFile = geometryShaderFile;
    m_tesShaderFile      = tesShaderFile;
    m_tcsShaderFile      = tcsShaderFile;
}

BOOL Shader::linkProgram()
{
    BOOL result = m_shaderCompiler.compileShader(
            m_vertexShaderFile, m_fragmentShaderFile,
            m_geometryShaderFile, m_tesShaderFile, m_tcsShaderFile, // Optional shader stages
            &m_shaderProgram);

    return result;
}

GLuint Shader::useProgram()
{
    glUseProgram(m_shaderProgram);

    return m_shaderProgram;
}

