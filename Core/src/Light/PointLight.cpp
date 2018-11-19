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
        PointLight::PointLight(
            const Math::Vector3& position,
            const Math::Vector3& color,
            float radius)
            :LightBase(POINT_LIGHT, color)
        {
            m_position = position;
            m_radius = radius;
        }

        PointLight::~PointLight()
        {

        }

        void PointLight::translate(
            const Math::Vector3& transVector)
        {
            m_position += transVector;
            // UpdateLightTrans();
        }

    }
}