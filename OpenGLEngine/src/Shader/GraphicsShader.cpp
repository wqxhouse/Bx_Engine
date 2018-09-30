#include "GraphicsShader.h"

GraphicsShader::GraphicsShader()
{
}

GraphicsShader::~GraphicsShader()
{
}

void GraphicsShader::setShaderFiles(
    char* const vertexShaderFile,
    char* const fragmentShaderFile,
    char* const geometryShaderFile,
    char* const tesShaderFile,
    char* const tcsShaderFile)
{
    m_vertexShaderFile = vertexShaderFile;
    m_fragmentShaderFile = fragmentShaderFile;
    m_geometryShaderFile = geometryShaderFile;
    m_tesShaderFile = tesShaderFile;
    m_tcsShaderFile = tcsShaderFile;
}

BOOL GraphicsShader::compileShaderProgram()
{
    BOOL result = m_shaderCompiler.compileShader(
        m_vertexShaderFile, m_fragmentShaderFile,
        m_geometryShaderFile, m_tesShaderFile, m_tcsShaderFile, // Optional shader stages
        &m_shaderProgram);

    return result;
}
