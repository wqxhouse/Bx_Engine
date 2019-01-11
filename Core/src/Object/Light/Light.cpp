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
        LightBase::LightBase(
            const UINT             objectId,
            const LightType        lightType,
            const LightCreateInfo& lightCreateInfo)
            : ObjectBase(objectId, lightCreateInfo.pTrans),
              m_lightType(lightType),
              enableLight(1.0f),
              reserve(0.0f),
              m_light_vec4(lightCreateInfo.lightColor, 0.0f)
        {
        }

        LightBase::~LightBase()
        {
        }
    }
}