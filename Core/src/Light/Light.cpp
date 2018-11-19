//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "Light.h"

namespace Object
{
    namespace Light
    {
        LightBase::LightBase()
            : m_lightType(UNKNOWN_LIGHT),
            enableLight(1.0f),
            reserve(0.0f),
            m_light_vec4(Math::Vector3(), 0.0f)
        {
        }

        LightBase::LightBase(
            const LightType lightType,
            const Math::Vector3& color)
            : m_lightType(lightType),
            enableLight(1.0f),
            reserve(0.0f),
            m_light_vec4(color, 0.0f)
        {
        }

        LightBase::~LightBase()
        {
        }
    }
}