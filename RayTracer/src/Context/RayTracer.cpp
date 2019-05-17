#include "RayTracer.h"
//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2019
//
//  All code licensed under the MIT license
//
//================================================================================================

namespace RayTracingEngine
{
    RayTracer::RayTracer(
        const Setting* const pSetting)
        : m_pSetting(pSetting)
    {
    }

    RayTracer::~RayTracer()
    {
    }
}