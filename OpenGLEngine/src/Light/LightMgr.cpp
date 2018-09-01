#include "LightMgr.h"

#define UNCREATED_LIGHT_UBO_HANDLE 0xFFFFFFFF

LightMgr::LightMgr()
    : m_lightUboHandle(UNCREATED_LIGHT_UBO_HANDLE)
{
    m_lightFlags.flags = 0;
}

LightMgr::~LightMgr()
{
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

    m_lightList.push_back(lightData);

    m_lightFlags.bitField.isRecreateLightUbo = TRUE;
}

void LightMgr::addPointLight(
    const Math::Vector3& position,
    const Math::Vector3& color,
    float radius)
{
    PointLight pointLight(position, color, radius);

    LightData lightData = *(reinterpret_cast<LightData*>(&pointLight));

    m_lightList.push_back(lightData);

    m_lightFlags.bitField.isRecreateLightUbo = TRUE;
}

void LightMgr::addSpotLight(
    const Math::Vector3& position,
    const Math::Vector3& direction,
    const Math::Vector3& color,
    float radius_in,
    float radius_out)
{
    SpotLight spotLight(position, direction, color, radius_in, radius_out);

    LightData lightData = *(reinterpret_cast<LightData*>(&spotLight));

    m_lightList.push_back(lightData);

    m_lightFlags.bitField.isRecreateLightUbo = TRUE;
}
