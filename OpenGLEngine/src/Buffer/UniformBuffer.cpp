#include "UniformBuffer.h"

UniformBuffer::UniformBuffer(
    const GLuint  uniformBufferIndex,
    const GLenum  usage,
    const GLsizei dataSize,
    const GLvoid* data)
    : m_uniformBufferIndex(uniformBufferIndex)
{
    {
        m_uniformDataBufferSize = dataSize;

        m_uniformDataBuffer = (GLubyte*)malloc(m_uniformDataBufferSize);

        if (data != NULL)
        {
            memcpy(m_uniformDataBuffer, data, m_uniformDataBufferSize);
        }

        glGenBuffers(1, &m_uniformBufferHandle);
        glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBufferHandle);
        glBufferData(GL_UNIFORM_BUFFER, m_uniformDataBufferSize, m_uniformDataBuffer, usage);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}

UniformBuffer::UniformBuffer(
    const GLuint                   uniformBufferIndex,
    const GLuint                   program,
    GLchar*                        uniformBlockName,
    const UINT                     memberCount,
    const UniformBlockMemberData*  uniformBlockMembers,
    const GLenum                   usage)
    : m_uniformBufferIndex(uniformBufferIndex)
{
    m_uniformBufferIndex = glGetUniformBlockIndex(program, uniformBlockName);
    if (m_uniformBufferIndex != -1)
    {
        //m_program               = program;
        //m_uniformBlockName      = uniformBlockName;
        m_memberCount           = memberCount;

        glGetActiveUniformBlockiv(program, m_uniformBufferIndex,
                                  GL_UNIFORM_BLOCK_DATA_SIZE, &m_uniformDataBufferSize);

        m_uniformDataBuffer = (GLubyte*)malloc(m_uniformDataBufferSize);

        m_memberNames = new GLchar*[memberCount];
        for (UINT i = 0; i < m_memberCount; ++i)
        {
            m_memberNames[i] = uniformBlockMembers[i].uniformBlockMember;
        }

        m_indices = new GLuint[m_memberCount];
        m_offsets = new GLint[m_memberCount];

        glGetUniformIndices(program, m_memberCount, m_memberNames, m_indices);
        glGetActiveUniformsiv(program, m_memberCount, m_indices, GL_UNIFORM_OFFSET, m_offsets);

        for (UINT i = 0; i < memberCount; ++i)
        {
            memcpy(m_uniformDataBuffer + m_offsets[i], 
                   uniformBlockMembers[i].data, uniformBlockMembers[i].dataSize);
        }

        glGenBuffers(1, &m_uniformBufferHandle);
        glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBufferHandle);
        glBufferData(GL_UNIFORM_BUFFER, m_uniformDataBufferSize, m_uniformDataBuffer, usage);
        glBindBufferBase(GL_UNIFORM_BUFFER, m_uniformBufferIndex, m_uniformBufferHandle);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    else
    {
        assert("Can't find the uniform buffer in the shader program.");
    }
}

UniformBuffer::~UniformBuffer()
{
    glDeleteBuffers(1, &m_uniformBufferHandle);

    SafeDeleteArray(m_indices);
    SafeDeleteArray(m_offsets);
    SafeDeleteArray(m_memberNames);

    SafeFree(m_uniformDataBuffer);

    for (UINT i = 0; i < m_memberCount; ++i)
    {
        SafeDelete(m_memberNames[i]);
    }
}

void UniformBuffer::update(const GLvoid * data)
{
    assert(data != NULL);

    memcpy(m_uniformDataBuffer, data, m_uniformDataBufferSize);

    glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBufferHandle);
    glBufferData(GL_UNIFORM_BUFFER, m_uniformDataBufferSize, m_uniformDataBuffer, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::update(
    const GLsizei dataSize,
    const GLvoid * data)
{
    assert(data != NULL);

    m_uniformDataBufferSize = dataSize;
    memcpy(m_uniformDataBuffer, data, dataSize);

    glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBufferHandle);
    glBufferData(GL_UNIFORM_BUFFER, m_uniformDataBufferSize, m_uniformDataBuffer, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::update(
    const UniformBlockMemberData* uniformBlockMembers)
{
    for (UINT i = 0; i < m_memberCount; ++i)
    {
        memcpy(m_uniformDataBuffer + m_offsets[i],
               uniformBlockMembers[i].data, uniformBlockMembers[i].dataSize);
    }

    glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBufferHandle);
    glBufferData(GL_UNIFORM_BUFFER, m_uniformDataBufferSize, m_uniformDataBuffer, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::update(
    const GLuint                   program,
    GLchar*                        uniformBlockName,
    const UINT                     memberCount,
    const UniformBlockMemberData*  uniformBlockMembers,
    GLenum                   usage)
{
    m_uniformBufferIndex = glGetUniformBlockIndex(program, uniformBlockName);
    if (m_uniformBufferIndex != -1)
    {
        //m_program          = program;
        //m_uniformBlockName = uniformBlockName;
        m_memberCount      = memberCount;

        glGetActiveUniformBlockiv(program, m_uniformBufferIndex,
            GL_UNIFORM_BLOCK_DATA_SIZE, &m_uniformDataBufferSize);

        SafeDelete(m_uniformDataBuffer);

        m_uniformDataBuffer = new GLubyte[m_uniformDataBufferSize];

        for (UINT i = 0; i < m_memberCount; ++i)
        {
            SafeDelete(m_memberNames[i]);
        }
        m_memberNames = new GLchar*[memberCount];

        for (UINT i = 0; i < memberCount; ++i)
        {
            m_memberNames[i] = uniformBlockMembers[i].uniformBlockMember;
        }

        glGetUniformIndices(program, m_memberCount, m_memberNames, m_indices);
        glGetActiveUniformsiv(program, m_memberCount, m_indices, GL_UNIFORM_OFFSET, m_offsets);

        for (UINT i = 0; i < memberCount; ++i)
        {
            memcpy(m_uniformDataBuffer + m_offsets[i],
                uniformBlockMembers[i].data, uniformBlockMembers[i].dataSize);
        }

        glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBufferHandle);
        glBufferData(GL_UNIFORM_BUFFER, m_uniformDataBufferSize, m_uniformDataBuffer, GL_STATIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, m_uniformBufferIndex, m_uniformBufferHandle);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    else
    {
        assert("Can't find the uniform buffer in the shader program.");
    }
}
