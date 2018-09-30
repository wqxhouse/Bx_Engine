#include "ComputeShader.h"

#define DEFAULT_GROUP_SIZE_X 16
#define DEFAULT_GROUP_SIZE_Y 16
#define DEFAULT_GROUP_SIZE_Z 1

ComputeShader::ComputeShader()
    : threadGroupChangeFlags(0)
{
    groupSize[0] = DEFAULT_GROUP_SIZE_X;
    groupSize[1] = DEFAULT_GROUP_SIZE_Y;
    groupSize[2] = DEFAULT_GROUP_SIZE_Z;
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

    glDispatchComputeGroupSizeARB(
        groupNum[0], groupNum[1], groupNum[2],
        groupSize[0], groupSize[1], groupSize[2]
    );
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    FinishProgram();
}

void ComputeShader::setTotalThreadSize(
    const UINT x,
    const UINT y,
    const UINT z)
{
    totalInvocationSize[0] = x;
    totalInvocationSize[1] = y;
    totalInvocationSize[2] = z;
}

void ComputeShader::setThreadGroupSize(
    const UINT x,
    const UINT y,
    const UINT z)
{
    groupSize[0] = x;
    groupSize[1] = y;
    groupSize[2] = z;
}

void ComputeShader::calGroupNum()
{
    if (threadGroupChangeFlags != 0)
    {
        groupNum[0] = totalInvocationSize[0] / groupSize[0];
        groupNum[1] = totalInvocationSize[1] / groupSize[1];
        groupNum[2] = totalInvocationSize[2] / groupSize[2];

        threadGroupChangeFlags = 0;
    }
}
