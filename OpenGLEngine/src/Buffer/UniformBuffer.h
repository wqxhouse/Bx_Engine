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
    UniformBuffer(
        const GLuint  uniformBufferIndex,
        const GLenum  usage,
        const GLsizei dataSize,
        const GLvoid* data);

    UniformBuffer(
        const GLuint                   uniformBufferIndex,
        const GLuint                   program,
        GLchar*                        uniformBlockName,
        const UINT                     memberCount,
        const UniformBlockMemberData*  uniformBlockMembers,
        const GLenum                   usage);

    ~UniformBuffer();

    void update(const GLvoid* data);
    void update(const GLsizei dataSize, const GLvoid* data);
    void update(const UniformBlockMemberData*  uniformBlockMembers);
    void update(
        const GLuint                   program,
        GLchar*                        uniformBlockName,
        const UINT                     memberCount,
        const UniformBlockMemberData*  uniformBlockMembers,
        const GLenum                   usage);

    inline GLuint   GetUniformbufferHandle() const { return m_uniformBufferHandle; }
    inline GLubyte* GetUniformBufferData()   const { return m_uniformDataBuffer;   }

private:
    GLuint        m_uniformBufferIndex;

    GLuint        m_uniformBufferHandle;
    GLubyte*      m_uniformDataBuffer;
    GLint         m_uniformDataBufferSize;

    //GLuint        m_program;
    //GLchar*       m_uniformBlockName;
    GLchar**      m_memberNames;
    UINT          m_memberCount;

    GLuint*       m_indices;
    GLint*        m_offsets;
};
