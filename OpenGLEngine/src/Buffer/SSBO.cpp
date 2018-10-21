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
    m_bindingPoint = bindingPoint;

    glGenBuffers(1, &m_ssboHandle);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboHandle);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, bufUsage);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, m_ssboHandle);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void* Ssbo::getData()
{
    void* pData = NULL;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboHandle); CHECK_GL_ERROR
    
    pData = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY); CHECK_GL_ERROR

    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER); CHECK_GL_ERROR

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); CHECK_GL_ERROR

    return pData;
}

void Ssbo::setData(
    const UINT  size,
    const void* data)
{
    void* pData = NULL;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboHandle);

    pData = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

    memcpy(pData, data, size);

    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
