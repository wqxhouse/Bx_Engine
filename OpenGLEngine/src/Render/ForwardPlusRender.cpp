#include "ForwardPlusRender.h"

using Math::Vector4;

ForwardPlusRender::ForwardPlusRender()
{
    m_frustums[0] = { 0.0f, 0.0f, 0.0f, 0.0f };
    m_frustums[1] = { 1.0f, 1.0f, 1.0f, 1.0f };
    m_frustums[2] = { 1.0f, 1.0f, 1.0f, 1.0f };
    m_frustums[3] = { 1.0f, 1.0f, 1.0f, 1.0f };
}

ForwardPlusRender::~ForwardPlusRender()
{
}

BOOL ForwardPlusRender::initialize()
{
    BOOL result = TRUE;

    m_gridFrustumComputeShader.setComputeShader("GridFrustumsCompute.cs"); CHECK_GL_ERROR
    result = m_gridFrustumComputeShader.compileComputeShader(); 

    assert(result == TRUE);

    m_gridFrustumComputeShader.useProgram(); CHECK_GL_ERROR

    glGenBuffers(1, &m_frustumVerticesSsbo); 
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_frustumVerticesSsbo); 
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(m_frustums), static_cast<void*>(&(m_frustums[0])), GL_DYNAMIC_COPY); 
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_frustumVerticesSsbo); 
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);    

    m_gridFrustumComputeShader.FinishProgram();

    return result;
}

void ForwardPlusRender::update()
{

}

void ForwardPlusRender::draw()
{
    m_gridFrustumComputeShader.useProgram(); CHECK_GL_ERROR

    glDispatchCompute(1, 1, 1); CHECK_GL_ERROR
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); CHECK_GL_ERROR

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0); CHECK_GL_ERROR

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_frustumVerticesSsbo); CHECK_GL_ERROR
    Vector4* gpuFrustumsPtr =
        static_cast<Vector4*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE)); CHECK_GL_ERROR

    /*for (int i = 0; i < 4; ++i)
    {
        gpuFrustumsPtr[i] = m_frustums[i];
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    Vector4* gpuFrustumsPtr2 =
        static_cast<Vector4*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_MAP_WRITE_BIT | GL_MAP_READ_BIT));*/

    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER); CHECK_GL_ERROR

    m_gridFrustumComputeShader.FinishProgram();
}
