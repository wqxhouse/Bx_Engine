#include "ComputeShader.h"

#define DEFAULT_GROUP_SIZE_X 16
#define DEFAULT_GROUP_SIZE_Y 16
#define DEFAULT_GROUP_SIZE_Z 1

ComputeShader::ComputeShader()
    : threadGroupChangeFlags(0),
      useComputeVariableGroupSize(TRUE)
{
    m_groupSize[0] = DEFAULT_GROUP_SIZE_X;
    m_groupSize[1] = DEFAULT_GROUP_SIZE_Y;
    m_groupSize[2] = DEFAULT_GROUP_SIZE_Z;
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

void ComputeShader::compute()
{
    useProgram();

    if (useComputeVariableGroupSize == FALSE)
    {
        glDispatchCompute(m_groupNum[0], m_groupNum[1], m_groupNum[2]);
    }
    else
    {
        glDispatchComputeGroupSizeARB(m_groupNum[0], m_groupNum[1], m_groupNum[2],
                                      m_groupSize[0], m_groupSize[1], m_groupSize[2]);
    }

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    FinishProgram();
}

void ComputeShader::setTotalThreadSize(
    const UINT x,
    const UINT y,
    const UINT z)
{
    m_totalInvocationSize[0] = x;
    m_totalInvocationSize[1] = y;
    m_totalInvocationSize[2] = z;
}

void ComputeShader::setThreadGroupSize(
    const UINT x,
    const UINT y,
    const UINT z)
{
    m_groupSize[0] = x;
    m_groupSize[1] = y;
    m_groupSize[2] = z;
}

void ComputeShader::calGroupNum()
{
    if (threadGroupChangeFlags != 0)
    {
        m_groupNum[0] = m_totalInvocationSize[0] / m_groupSize[0];
        m_groupNum[1] = m_totalInvocationSize[1] / m_groupSize[1];
        m_groupNum[2] = m_totalInvocationSize[2] / m_groupSize[2];

        threadGroupChangeFlags = 0;
    }
}
