#include "ComputeShader.h"

ComputeShader::ComputeShader()
{
}

ComputeShader::~ComputeShader()
{
}

BOOL ComputeShader::compileShaderProgram()
{
    BOOL result = TRUE;

    if (m_computeShaderFile != NULL)
    {
        result = m_shaderCompiler.compileComputeShader(m_computeShaderFile, &m_shaderProgram);
    }
    else
    {
        result = FALSE;
    }

    return result;
}
