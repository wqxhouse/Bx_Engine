#include "SSBO.h"

Ssbo::Ssbo()
{    
}

Ssbo::~Ssbo()
{
    glDeleteBuffers(1, &m_ssboHandle);
}

void Ssbo::createStaticSsbo(
    const UINT   size,
    const void*  data,
    const GLenum bufFlags,
    const UINT   bindingPoint)
{
    m_bindingPoint = bindingPoint;

    glGenBuffers(1, &m_ssboHandle);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboHandle);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, size, data, bufFlags);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, m_ssboHandle);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Ssbo::createDynamicSsbo(
    const UINT   size,
    const void*  data,
    const GLenum bufUsage,
    const UINT   bindingPoint)
{
    glGenBuffers(1, &m_ssboHandle);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboHandle);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, bufUsage);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, m_ssboHandle);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void* Ssbo::getData()
{
    void* pData = NULL;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboHandle);
    
    pData = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    return pData;
}

void* Ssbo::setData(
    const UINT  size,
    const void* data)
{
    void* pData = NULL;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboHandle);

    pData = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

    pData = memcpy(pData, data, size);

    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    return pData;
}
