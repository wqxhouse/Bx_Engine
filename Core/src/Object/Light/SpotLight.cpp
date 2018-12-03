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
        SpotLight::SpotLight(
            const Math::Vector3& position,
            const Math::Vector3& direction,
            const Math::Vector3& color,
            float distance,
            float in_angle,
            float out_angle)
            : LightBase(SPOT_LIGHT,
                        color,
                        new Trans(position,
                                  position + direction,
                                  Math::Vector3(0.0f, 1.0f, 0.0f))),
              m_position(position),
              m_direction(direction)
        {
            assert(in_angle <= out_angle);

            m_in_cosTheta = std::cos(in_angle * 0.5f);
            m_out_cosTheta = std::cos(out_angle * 0.5f);

            assert(m_in_cosTheta > 0 && m_out_cosTheta > 0);

            // reserve = distance; // Use the reserve to store distance
        }

        SpotLight::~SpotLight()
        {
        }

        void SpotLight::translate(
            const Math::Vector3& transVector)
        {
            m_position += transVector;
            // UpdateLightTrans();
        }

        void SpotLight::rotate(
            const Math::Vector3 & axis,
            const float angle)
        {
            m_direction = Math::rotate(m_direction, axis, angle);
            // UpdateLightTrans();
        }
    }
}