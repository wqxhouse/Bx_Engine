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
            const SpotLightCreateInfo& spotLightCreateInfo)
            : LightBase(SPOT_LIGHT, static_cast<LightCreateInfo>(spotLightCreateInfo)),
              m_position(m_pTrans->GetPos()),
              m_direction(m_pTrans->GetFront())
        {
            assert(spotLightCreateInfo.in_angle <= spotLightCreateInfo.out_angle);

            m_in_cosTheta  = std::cos(spotLightCreateInfo.in_angle * 0.5f);
            m_out_cosTheta = std::cos(spotLightCreateInfo.out_angle * 0.5f);

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