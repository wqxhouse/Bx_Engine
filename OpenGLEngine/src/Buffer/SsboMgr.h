#pragma once

#include "Ssbo.h"

#define DEFAULT_SSBO_BINDING_POINT_NUM GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS

class SsboMgr
{
public:
    SsboMgr(const UINT bindingPointNum = DEFAULT_SSBO_BINDING_POINT_NUM);

    ~SsboMgr();

    GLuint addStaticSsbo(
        const UINT   size,
        const void*  data,
        const GLenum bufFlags,
        const UINT   bindingPoint);

    GLuint addDynamicSsbo(
        const UINT   size,
        const void*  data,
        const GLenum bufUsage,
        const UINT   bindingPoint);

    void updateBindingPoint(
        const UINT   size,
        const void*  data,
        const GLenum usage,
        const UINT   bindingPoint);

    void* GetSsboData(const UINT bindingPoint);
    void* SetSsboData(
        const UINT  bindingPoint,
        const UINT  size,
        const void* data);

    inline Ssbo* GetSsbo(const UINT bindingPoint) const { return m_pSsboList[bindingPoint]; }

private:
    std::vector<Ssbo*> m_pSsboList;
};