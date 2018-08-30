#include "LightMgr.h"

LightMgr::LightMgr()
    : m_lightUboHandle(0xFFFFFFFF)
{
}

LightMgr::~LightMgr()
{
}

void LightMgr::createLightUbo(
    UniformBufferMgr* m_pUboMgr)
{
    m_lightUboHandle =
        m_pUboMgr->createUniformBuffer(GL_DYNAMIC_DRAW,
                                       GetLightDataSize(),
                                       GetLightData());
}

void LightMgr::updateLightUbo(
    UniformBufferMgr* m_pUboMgr)
{
    m_pUboMgr->updateUniformBufferData(
        m_lightUboHandle, GetLightDataSize(), GetLightData());
}

void LightMgr::translateLight(
    const UINT           index,
    const Math::Vector3& transVector)
{
}

void LightMgr::rotateLight(
    const UINT           index,
    const Math::Vector3& axis,
    const float          angle)
{
    Light* pLight = reinterpret_cast<Light*>(&(m_lightList[index]));

    switch (pLight->GetLightType())
    {
        case DIRECTIONAL_LIGHT:
        {
            static_cast<DirectionalLight*>(pLight)->rotate(axis, angle);
            break;
        }
        case POINT_LIGHT:
        {
            break;
        }
        case SPOT_LIGHT:
        {
            break;
        }
        default:
            printf("Unknown light type!\n");
            assert(FALSE);
            break;
    }
}

void LightMgr::addDirectionalLight(
    const Math::Vector3& direction,
    const Math::Vector3& color)
{
    DirectionalLight directionalLight(direction, color);

    LightData lightData = *(reinterpret_cast<LightData*>(&directionalLight));

    m_lightList.push_back(lightData);
}

void LightMgr::addPointLight(
    const Math::Vector3& position,
    const Math::Vector3& color,
    float radius)
{
    PointLight pointLight(position, color, radius);

    LightData lightData = *(reinterpret_cast<LightData*>(&pointLight));

    m_lightList.push_back(lightData);
}

void LightMgr::addSpotLight(
    const Math::Vector3& position,
    const Math::Vector3& color,
    float radius_in,
    float radius_out)
{
    SpotLight spotLight(position, color, radius_in, radius_out);

    LightData lightData = *(reinterpret_cast<LightData*>(&spotLight));

    m_lightList.push_back(lightData);
}
