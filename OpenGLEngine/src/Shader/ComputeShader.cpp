//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "ComputeShader.h"

#define DEFAULT_GROUP_SIZE_X 16
#define DEFAULT_GROUP_SIZE_Y 16
#define DEFAULT_GROUP_SIZE_Z 1

ComputeShader::ComputeShader()
    :useComputeVariableGroupSize(TRUE)
{
    m_groupSize[0] = DEFAULT_GROUP_SIZE_X;
    m_groupSize[1] = DEFAULT_GROUP_SIZE_Y;
    m_groupSize[2] = DEFAULT_GROUP_SIZE_Z;

    threadGroupChangeFlags.value = 0;
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
    calGroupNum();

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
}

void ComputeShader::setTotalThreadSize(
    const UINT x,
    const UINT y,
    const UINT z)
{
    m_totalInvocationSize[0] = x;
    m_totalInvocationSize[1] = y;
    m_totalInvocationSize[2] = z;

    threadGroupChangeFlags.bits.totalInvocationSizeChange = TRUE;
}

void ComputeShader::setThreadGroupSize(
    const UINT x,
    const UINT y,
    const UINT z)
{
    m_groupSize[0] = x;
    m_groupSize[1] = y;
    m_groupSize[2] = z;

    threadGroupChangeFlags.bits.groupSizeChange = TRUE;
}

void ComputeShader::calGroupNum()
{
    if (threadGroupChangeFlags.value != 0)
    {
        for (int i = 0; i < 3; ++i)
        {
            m_groupNum[i] =
                static_cast<int>(std::ceil(static_cast<float>(m_totalInvocationSize[i]) / static_cast<float>(m_groupSize[i])));
        }
        threadGroupChangeFlags.value = 0;
    }
}
