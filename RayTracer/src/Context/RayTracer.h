//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2019
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#if BX_RAYTRACER

#include <Core/PCH.h>
#include <Setting/Setting.h>

namespace RayTracingEngine
{
    class RayTracer
    {
    public:
        RayTracer(const Setting* const pSetting);
        ~RayTracer();

    private:
        const Setting* const m_pSetting;
    };
}

#endif