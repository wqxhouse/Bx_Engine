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
            const UINT                  objectId,
            const PointLightCreateInfo& pointLightCreateInfo)
            : LightBase(objectId,
                        POINT_LIGHT,
                        static_cast<LightCreateInfo>(pointLightCreateInfo))
        {
            m_position = m_pTrans->GetPos();
            m_radius   = pointLightCreateInfo.radius;
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