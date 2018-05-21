#include "UniformBuffer.h"
#include "UniformBufferMgr.h"

UniformBufferMgr::UniformBufferMgr(const UINT uniformBufferCount)
    : m_maxUboCount(uniformBufferCount)
{
    m_pUniformBindBlockList.reserve(uniformBufferCount);
}

UniformBufferMgr::~UniformBufferMgr()
{
    for (UniformBuffer* pUniformBuffer : m_pUniformBindBlockList)
    {
        SafeDelete(pUniformBuffer);
    }
}

UINT UniformBufferMgr::createUniformBuffer(
    const GLuint                  program,
    const GLchar*                 uniformBlockName,
    const UINT                    memberCount,
    const UniformBlockMemberData* uniformBlockMembers,
    const GLenum                  usage)
{
    UINT uniformBufferBindIndex = m_pUniformBindBlockList.size();
    assert(uniformBufferBindIndex < m_maxUboCount);

    //TODO
    assert("Hasn't implemented!");

    return uniformBufferBindIndex;
}

UINT UniformBufferMgr::createUniformBuffer(
    const GLenum  usage,
    const GLsizei dataSize,
    const GLvoid* data)
{
    UINT uniformBufferBindIndex = m_pUniformBindBlockList.size();
    assert(uniformBufferBindIndex < m_maxUboCount);

    UniformBuffer* newUniformBufferPtr =
        new UniformBuffer(uniformBufferBindIndex, usage, dataSize, data);
    
    m_pUniformBindBlockList.push_back(newUniformBufferPtr);

    return uniformBufferBindIndex;
}

void UniformBufferMgr::bindUniformBuffer(
    const UINT    uniformBufferBindIndex,
    const GLuint  program,
    const GLchar* uniformBufferBlockName)
{
    const GLuint shaderLocation = glGetUniformBlockIndex(program, uniformBufferBlockName);

    if (shaderLocation != -1)
    {
        glUniformBlockBinding(program, shaderLocation, uniformBufferBindIndex);

        UniformBuffer* pUniformBuffer = m_pUniformBindBlockList[uniformBufferBindIndex];
        glBindBufferBase(GL_UNIFORM_BUFFER, uniformBufferBindIndex, pUniformBuffer->getUniformbufferHandle());
    }
    else
    {
        assert("Can't find the uniform buffer in the shader program.");
    }
}

void UniformBufferMgr::updateUniformBufferData(
    const UINT    uniformBufferBindIndex,
    const GLsizei dataSize,
    const GLvoid* data)
{
    UniformBuffer* pUniformBuffer = m_pUniformBindBlockList[uniformBufferBindIndex];
    pUniformBuffer->update(dataSize, data);
}