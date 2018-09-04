#include "ShadowMgr.h"
#include "../Context/Scene.h"

ShadowMgr::ShadowMgr(Scene* pScene)
    : m_pScene(pScene),
      m_pShadowTexture(NULL)
{
}

ShadowMgr::~ShadowMgr()
{
    //SafeDelete(m_pShadowTexture);
    for (ShadowMap* pShadowMap : m_pShadowMapList)
    {
        SafeDelete(pShadowMap);
    }
}

BOOL ShadowMgr::initialize()
{
    BOOL status = TRUE;

    Setting* pSetting = m_pScene->GetSetting();

    // Create texture2D array with x,y and layers
    m_pShadowTexture = new Texture3D(pSetting->m_graphicsSetting.shadowResolution.width,
                                     pSetting->m_graphicsSetting.shadowResolution.height,
                                     MAX_SHADOW_NUM,
                                     1,                                         // Samples
                                     GL_DEPTH_COMPONENT,
                                     GL_DEPTH_COMPONENT,
                                     GL_FLOAT,
                                     GL_CLAMP_TO_EDGE,
                                     FALSE, NULL);

    if (m_pShadowTexture == NULL)
    {
        status = FALSE;
    }

    return status;
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
    Setting*   pSetting   = m_pScene->GetSetting();
    ShadowMap* pShadowMap = m_pShadowMapList[i];

    glCullFace(GL_FRONT);
    if (pSetting->m_graphicsSetting.shadowCasting == FALSE)
    {
        glDepthFunc(GL_NEVER);
    }
    GLenum error = glGetError();
    m_shadowFbo.attachTexture3D(GL_TEXTURE0, GL_DEPTH_ATTACHMENT, m_pShadowTexture, i);
    error = glGetError();
    m_shadowFbo.drawFramebuffer();
    error = glGetError();

    /*glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    pShadowMap->drawShadowMap(m_pScene);

    glDrawBuffer(GL_FRONT);
    glReadBuffer(GL_FRONT);*/

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

void ShadowMgr::readShadowMap(
    const GLenum       texUnit,
    const GLuint       shaderProgram,
    const std::string& samplerName,
    const UINT         samplerIndex)
{
    m_pShadowTexture->bindTexture(texUnit, shaderProgram, samplerName, samplerIndex);
}
