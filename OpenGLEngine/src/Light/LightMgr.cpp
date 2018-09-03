#include "LightMgr.h"

#define UNCREATED_LIGHT_UBO_HANDLE 0xFFFFFFFF

LightMgr::LightMgr(Scene* pScene)
    : m_pScene(pScene),
      m_lightUboHandle(UNCREATED_LIGHT_UBO_HANDLE),
      m_shadowMgr(pScene)
{
    m_lightFlags.flags = 0;
}

LightMgr::~LightMgr()
{
    //for (size_t i = 0; i < m_lightList.size(); i++)
    //{
    //    Light* pLight = GetLight(i);

    //    // TODO: Avoiding manually release the shadow map here
    //    SafeDelete(pLight->GetShadowMap());
    //}

    m_lightList.clear();
}

void LightMgr::createLightUbo(
    UniformBufferMgr* pUboMgr)
{
    if (m_lightUboHandle == UNCREATED_LIGHT_UBO_HANDLE)
    { 
        m_lightUboHandle =
            pUboMgr->createUniformBuffer(GL_DYNAMIC_DRAW,
                                         GetLightDataSize(),
                                         GetLightData());
    }
    else
    {
        pUboMgr->recreateUniformBuffer(m_lightUboHandle,
                                       GL_DYNAMIC_DRAW,
                                       GetLightDataSize(),
                                       GetLightData());
    }

    m_lightFlags.bitField.isRecreateLightUbo = FALSE;
}

void LightMgr::updateLightUbo(
    UniformBufferMgr* pUboMgr,
    const GLuint      shaderProgram,
    const GLchar*     uniformBufferBlockName)
{
    if (m_lightFlags.bitField.isRecreateLightUbo == TRUE)
    {
        createLightUbo(pUboMgr);
        bindLightUbo(pUboMgr, shaderProgram, uniformBufferBlockName);
    }
    else if (m_lightFlags.bitField.isUpdateLightUbo == TRUE)
    {
        pUboMgr->updateUniformBufferData(
            m_lightUboHandle, GetLightDataSize(), GetLightData());
    }

    // Reset flags
    m_lightFlags.flags = 0;
}

void LightMgr::bindLightUbo(
    UniformBufferMgr* m_pUboMgr,
    const GLuint      shaderProgram,
    const GLchar*     uniformBufferBlockName)
{
    m_pUboMgr->bindUniformBuffer(m_lightUboHandle, shaderProgram, uniformBufferBlockName);
}

void LightMgr::translateLight(
    const UINT           index,
    const Math::Vector3& transVector)
{
    Light* pLight = reinterpret_cast<Light*>(&(m_lightList[index]));

    assert(pLight != NULL);

    if (pLight != NULL)
    {
        pLight->translate(transVector);

        m_lightFlags.bitField.isUpdateLightUbo = TRUE;
    }
}

void LightMgr::rotateLight(
    const UINT           index,
    const Math::Vector3& axis,
    const float          angle)
{
    Light* pLight = reinterpret_cast<Light*>(&(m_lightList[index]));

    assert(pLight != NULL);

    if (pLight != NULL)
    {
        pLight->rotate(axis, angle);

        m_lightFlags.bitField.isUpdateLightUbo = TRUE;
    }
}

void LightMgr::addDirectionalLight(
    const Math::Vector3& direction,
    const Math::Vector3& color)
{
    DirectionalLight directionalLight(direction, color);

    LightData lightData = *(reinterpret_cast<LightData*>(&directionalLight));
    Light*    pLight    = reinterpret_cast<Light*>(&lightData);

    m_shadowMgr.createShadowMap(pLight);

    m_lightList.push_back(lightData);

    m_lightFlags.bitField.isRecreateLightUbo = TRUE;
}

void LightMgr::addPointLight(
    const Math::Vector3& position,
    const Math::Vector3& color,
    const float radius)
{
    PointLight pointLight(position, color, radius);

    LightData lightData = *(reinterpret_cast<LightData*>(&pointLight));
    Light*    pLight    = reinterpret_cast<Light*>(&lightData);

    m_shadowMgr.createShadowMap(pLight);

    m_lightList.push_back(lightData);

    m_lightFlags.bitField.isRecreateLightUbo = TRUE;
}

void LightMgr::addSpotLight(
    const Math::Vector3& position,
    const Math::Vector3& direction,
    const Math::Vector3& color,
    const float          distance,
    const float          in_angle,
    const float          out_angle)
{
    SpotLight spotLight(position, direction, color, distance, in_angle, out_angle);

    LightData lightData = *(reinterpret_cast<LightData*>(&spotLight));
    Light*    pLight    = reinterpret_cast<Light*>(&lightData);

    m_shadowMgr.createShadowMap(pLight);

    m_lightList.push_back(lightData);

    m_lightFlags.bitField.isRecreateLightUbo = TRUE;
}
