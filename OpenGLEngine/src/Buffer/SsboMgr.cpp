#include "SsboMgr.h"

SsboMgr::SsboMgr(
    const UINT bindingPointNum)
{
    m_pSsboList.resize(bindingPointNum);
}

SsboMgr::~SsboMgr()
{
    for (Ssbo* pSsbo : m_pSsboList)
    {
        SafeDelete(pSsbo);
    }
}

GLuint SsboMgr::addStaticSsbo(
    const UINT   size,
    const void*  data,
    const GLenum bufFlags,
    const UINT   bindingPoint)
{
    GLuint ssboHandle = INVALID_HANDLE;

    if (m_pSsboList[bindingPoint] == NULL)
    {
         Ssbo* pNewSsbo = new Ssbo();
         pNewSsbo->createStaticSsbo(size, data, bufFlags, bindingPoint);

         m_pSsboList[bindingPoint] = pNewSsbo;

         ssboHandle = pNewSsbo->GetSsboHandle();
    }
    else
    {
        printf("Binding point %u has already be binded.\n", bindingPoint);
        assert(FALSE);
    }

    return ssboHandle;
}

GLuint SsboMgr::addDynamicSsbo(
    const UINT   size,
    const void*  data,
    const GLenum bufUsage,
    const UINT   bindingPoint)
{
    GLuint ssboHandle = INVALID_HANDLE;

    if (m_pSsboList[bindingPoint] == NULL)
    {
        Ssbo* pNewSsbo = new Ssbo();
        pNewSsbo->createDynamicSsbo(size, data, bufUsage, bindingPoint);

        m_pSsboList[bindingPoint] = pNewSsbo;

        ssboHandle = pNewSsbo->GetSsboHandle();
    }
    else
    {
        printf("Binding point %u has already be binded.\n", bindingPoint);
        assert(FALSE);
    }

    return ssboHandle;
}

void SsboMgr::updateBindingPoint(
    const UINT   size,
    const void*  data,
    const GLenum usage,
    const UINT   bindingPoint)
{
    SafeDelete(m_pSsboList[bindingPoint]);
    Ssbo* pNewSsbo = new Ssbo();
    pNewSsbo->createDynamicSsbo(size, data, usage, bindingPoint);

    m_pSsboList[bindingPoint] = pNewSsbo;
}

void* SsboMgr::GetSsboData(const UINT bindingPoint)
{
    Ssbo* pSsbo = m_pSsboList[bindingPoint];
    return pSsbo->getData();
}

void* SsboMgr::SetSsboData(
    const UINT  bindingPoint,
    const UINT  size,
    const void* data)
{
    Ssbo* pSsbo = m_pSsboList[bindingPoint];
    return pSsbo->setData(size, data);
}
