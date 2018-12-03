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
            const float          radius)
            : LightBase(POINT_LIGHT,
                        color,
                        new Trans(position,
                                  Math::Vector3(0.0f, 0.0f, 0.0f),
                                  Math::Vector3(0.0f, 1.0f, 0.0f)))
        {
            m_position = position;
            m_radius   = radius;
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