#include "ForwardPlusRender.h"

ForwardPlusRender::ForwardPlusRender()
{
}

ForwardPlusRender::~ForwardPlusRender()
{
}

BOOL ForwardPlusRender::initialize()
{
    BOOL result = TRUE;

    m_gridFrustumComputeShader.setComputeShader("GridFrustumsCompute.cs");
    result = m_gridFrustumComputeShader.compileComputeShader();

    assert(result == TRUE);

    glGenBuffers(1, &m_frustumVerticesSsbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_frustumVerticesSsbo);



    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    return result;
}

void ForwardPlusRender::update()
{

}

void ForwardPlusRender::draw()
{
    m_gridFrustumComputeShader.useProgram();
    glDispatchCompute(1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    m_gridFrustumComputeShader.FinishProgram();
}
