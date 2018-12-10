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
        DirectionalLight::DirectionalLight(
            const Math::Vector3& direction,
            const Math::Vector3& color)
            : LightBase(DIRECTIONAL_LIGHT,
                        color,
                        new Trans(Math::Vector3(0.0f, 0.0f, 0.0f),
                                  direction,
                                  Math::Vector3(0.0f, 1.0f, 0.0f)))
        {
            m_direction = Math::Vector3::Normalize(direction);
            m_direction_vec4 = Math::Vector4(m_direction, 0.0f);
        }

        DirectionalLight::~DirectionalLight()
        {
        }

        void DirectionalLight::rotate(
            const Math::Vector3& axis,
            const float angle)
        {
            m_direction = Math::rotate(m_direction, axis, angle);
            // UpdateLightTrans();
        }
    }
}