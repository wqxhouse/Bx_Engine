#include "SSBO.h"

SSBO::SSBO(
    const UINT  size,
    const void* data,
    const UINT  bindingPoint)
{
}

SSBO::~SSBO()
{
    glDeleteBuffers(1, &m_ssboHandle);
}

BOOL SSBO::create()
{
    BOOL result = TRUE;

    return result;
}
