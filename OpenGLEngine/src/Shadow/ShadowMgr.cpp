#include "ShadowMgr.h"
#include "../Context/Scene.h"

ShadowMgr::ShadowMgr(Scene* pScene)
    : m_pScene(pScene),
      m_pTexture3D(NULL)
{
}

ShadowMgr::~ShadowMgr()
{
    SafeDelete(m_pTexture3D);
    for (ShadowMap* pShadowMap : m_pShadowMapList)
    {
        SafeDelete(pShadowMap);
    }
}

void ShadowMgr::initialize()
{
    Setting* pSetting = m_pScene->GetSetting();

    // Create texture2D array with x,y and layers
    m_pTexture3D = new Texture3D(pSetting->m_graphicsSetting.shadowResolution.width,
                                 pSetting->m_graphicsSetting.shadowResolution.height,
                                 MAX_SHADOW_NUM);
}

void ShadowMgr::createShadowMap(
    Light* pLight)
{
    ShadowMap* pShadowMap = new ShadowMap(m_pScene, pLight);
    m_pShadowMapList.push_back(pShadowMap);

    pLight->AttachShadowMap(pShadowMap);
    pShadowMap->initialize();
}

void ShadowMgr::updateShadowMap(
    const UINT i,
    Light*     pLight)
{
    ShadowMap* pShadowMap = m_pShadowMapList[i];
    pShadowMap->update(pLight);
}

void ShadowMgr::castShadow(
    const UINT i)   ///< Index of light for shadow
{
    Setting  * pSetting   = m_pScene->GetSetting();
    ShadowMap* pShadowMap = m_pShadowMapList[i];

    glCullFace(GL_FRONT);
    if (pSetting->m_graphicsSetting.shadowCasting == FALSE)
    {
        glDepthFunc(GL_NEVER);
    }

    m_shadowFbo.attachTexture3D(GL_TEXTURE0, GL_COLOR_ATTACHMENT0, m_pTexture3D, i);
    m_shadowFbo.drawFramebuffer();

    pShadowMap->drawShadowMap(m_pScene);

    glCullFace(GL_BACK);
    if (pSetting->m_graphicsSetting.shadowCasting == FALSE)
    {
        glDepthFunc(GL_LEQUAL);
    }

    m_shadowFbo.finishDrawFramebuffer();
}

void ShadowMgr::castShadowAll()
{
    for (size_t i = 0; i < m_pShadowMapList.size(); ++i)
    {
        castShadow(i);
    }
}
