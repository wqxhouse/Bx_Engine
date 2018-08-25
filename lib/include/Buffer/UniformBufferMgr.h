#pragma once

#include "UniformBuffer.h"

class UniformBufferMgr
{
public:
    UniformBufferMgr(const UINT uniformBufferCount = GL_MAX_UNIFORM_LOCATIONS);
    ~UniformBufferMgr();

    // Create uniform buffer with default memory saving layout
    // Return: Ubo bind index
    UINT createUniformBuffer(
        const GLuint                   program,
        const GLchar*                  uniformBlockName,
        const UINT                     memberCount,
        const UniformBlockMemberData*  uniformBlockMembers,
        const GLenum                   usage);

    // Create uniform buffer with std140 layout
    UINT createUniformBuffer(
        const GLenum  usage,
        const GLsizei dataSize,
        const GLvoid* data);

    void bindUniformBuffer(
        const UINT    uniformBufferBindIndex,
        const GLuint  program,
        const GLchar* uniformBufferBlockName);

    void updateUniformBufferData(
        const UINT uniformBufferBindIndex,
        const GLsizei dataSize,
        const GLvoid* data);

private:
    UINT m_maxUboCount;
    std::vector<UniformBuffer*> m_pUniformBindBlockList;
};