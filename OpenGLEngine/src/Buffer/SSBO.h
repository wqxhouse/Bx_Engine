#pragma once

#include "../Core/OpenGLPCH.h"

class SSBO
{
public:
    SSBO(const UINT  size,
         const void* data,
         const UINT  bindingPoint);

    ~SSBO();

    BOOL create();

private:
    GLuint m_ssboHandle;
    UINT   m_bindingPoint;
};