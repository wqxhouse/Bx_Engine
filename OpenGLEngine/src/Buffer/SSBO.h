#pragma once

#include "../Core/OpenGLPCH.h"

class Ssbo
{
public:
    Ssbo();

    ~Ssbo();

    void createStaticSsbo(
        const UINT   size,
        const void*  data,
        const GLenum bufFlags,
        const UINT   bindingPoint);

    void createDynamicSsbo(
        const UINT   size,
        const void*  data,
        const GLenum bufUsage,
        const UINT   bindingPoint);

    void* getData();
    void  setData(
        const UINT  size,
        const void* data);

    inline GLuint GetSsboHandle() const { return m_ssboHandle; }

private:
    GLuint m_ssboHandle;
    UINT   m_bindingPoint;
};