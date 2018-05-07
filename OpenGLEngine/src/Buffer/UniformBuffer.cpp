#include "UniformBuffer.h"

UniformBuffer::UniformBuffer(const GLuint                   program,
                             const GLchar*                  uniformBlockName,
                             const UINT                     memberCount,
                             const UniformBlockMemberData*  uniformBlockMembers)
{

}

UniformBuffer::~UniformBuffer()
{
    glDeleteBuffers(1, &m_uniformBufferHandle);
    SafeDelete(m_uniformDataBuffer);
}

void UniformBuffer::update(const UniformBlockMemberData * uniformBlockMembers)
{
}

void UniformBuffer::update(const GLuint                   program,
                           const GLchar*                  uniformBlockName,
                           const UINT                     memberCount,
                           const UniformBlockMemberData*  uniformBlockMembers)
{

}
