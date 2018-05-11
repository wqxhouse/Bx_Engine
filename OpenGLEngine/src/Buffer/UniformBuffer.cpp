#include "UniformBuffer.h"

UniformBuffer::UniformBuffer(
    const GLuint  program,
    GLchar*       uniformBlockName,
    const GLsizei dataSize,
    const GLvoid* data,
    const GLenum  usage,
    const GLuint  parag)
{
    m_uniformBufferIndex = glGetUniformBlockIndex(program, uniformBlockName);
    if (m_uniformBufferIndex != -1)
    {
        m_program = program;
        m_uniformBlockName = uniformBlockName;
        m_uniformDataBufferSize = dataSize;

        m_uniformDataBuffer = (GLubyte*)malloc(m_uniformDataBufferSize);//new GLubyte[m_uniformDataBufferSize];
        memcpy(m_uniformDataBuffer, data, m_uniformDataBufferSize);

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

UniformBuffer::UniformBuffer(
    const GLuint                   program,
    GLchar*                        uniformBlockName,
    const UINT                     memberCount,
    const UniformBlockMemberData*  uniformBlockMembers,
    const GLenum                   usage)
{
    m_uniformBufferIndex = glGetUniformBlockIndex(program, uniformBlockName);
    if (m_uniformBufferIndex != -1)
    {
        m_program               = program;
        m_uniformBlockName      = uniformBlockName;
        m_memberCount           = memberCount;

        glGetActiveUniformBlockiv(m_program, m_uniformBufferIndex,
                                  GL_UNIFORM_BLOCK_DATA_SIZE, &m_uniformDataBufferSize);

        m_uniformDataBuffer = new GLubyte[m_uniformDataBufferSize];

        m_memberNames = new GLchar*[memberCount];
        for (UINT i = 0; i < m_memberCount; ++i)
        {
            m_memberNames[i] = uniformBlockMembers[i].uniformBlockMember;
        }

        m_indices = new GLuint[m_memberCount];
        m_offsets = new GLint[m_memberCount];

        glGetUniformIndices(m_program, m_memberCount, m_memberNames, m_indices);
        glGetActiveUniformsiv(m_program, m_memberCount, m_indices, GL_UNIFORM_OFFSET, m_offsets);

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

    SafeDelete(m_uniformBlockName);
    SafeDeleteArray(m_indices);
    SafeDeleteArray(m_offsets);
    SafeDeleteArray(m_memberNames);
    //SafeDeleteArray(m_uniformDataBuffer);
    SafeFree(m_uniformDataBuffer);

    for (UINT i = 0; i < m_memberCount; ++i)
    {
        SafeDelete(m_memberNames[i]);
    }
}

void UniformBuffer::update(const GLvoid * data)
{
    memcpy(m_uniformDataBuffer, data, m_uniformDataBufferSize);

    glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBufferHandle);
    glBufferData(GL_UNIFORM_BUFFER, m_uniformDataBufferSize, m_uniformDataBuffer, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::update(const GLsizei dataSize, const GLvoid * data)
{
    m_uniformDataBufferSize = dataSize;
    memcpy(m_uniformDataBuffer, data, dataSize);

    glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBufferHandle);
    glBufferData(GL_UNIFORM_BUFFER, m_uniformDataBufferSize, m_uniformDataBuffer, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::update(const UniformBlockMemberData * uniformBlockMembers)
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
        m_program          = program;
        m_uniformBlockName = uniformBlockName;
        m_memberCount      = memberCount;

        glGetActiveUniformBlockiv(m_program, m_uniformBufferIndex,
            GL_UNIFORM_BLOCK_DATA_SIZE, &m_uniformDataBufferSize);

        delete[] m_uniformDataBuffer;
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

        glGetUniformIndices(m_program, m_memberCount, m_memberNames, m_indices);
        glGetActiveUniformsiv(m_program, m_memberCount, m_indices, GL_UNIFORM_OFFSET, m_offsets);

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
