#pragma once

#include "../Core/OpenGLPCH.h"

struct UniformBlockMemberData
{
    GLchar* uniformBlockMember;
    size_t  dataSize;
    void*   data;
};

class UniformBuffer
{
public:
    UniformBuffer(const GLuint                   program,
                  const GLchar*                  uniformBlockName,
                  const UINT                     memberCount,
                  const UniformBlockMemberData*  uniformBlockMembers);

    ~UniformBuffer();

    void update(const UniformBlockMemberData* uniformBlockMembers);
    void update(const GLuint                   program,
                const GLchar*                  uniformBlockName,
                const UINT                     memberCount,
                const UniformBlockMemberData*  uniformBlockMembers);

private:
    GLuint        m_uniformBufferHandle;
    GLubyte*      m_uniformDataBuffer;
    UINT          m_uniformDataBufferSize;

    GLuint        m_program;
    GLchar*       m_uniformBlockName;
    UINT          m_memberCount;
};

