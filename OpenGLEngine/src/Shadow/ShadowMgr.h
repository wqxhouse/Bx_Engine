#pragma once

#include "ShadowMap.h"

#define MAX_SHADOW_NUM MAX_LIGHT_NUM

class Scene;

class ShadowMgr
{
public:
    ShadowMgr(Scene* pScene);
    ~ShadowMgr();

    BOOL initialize();
    void createShadowMap(Light* pLight);
    void updateShadowMap(const UINT i, Light* pLight);
    void castShadow(const UINT i);

    void castShadowAll();

    void readShadowMap(
        const GLenum       texUnit,
        const GLuint       shaderProgram,
        const std::string& samplerName,
        const UINT         samplerIndex);

private:
    Scene*      m_pScene;

    Framebuffer m_shadowFbo;
    Texture3D*  m_pShadowTexture;

    std::vector<ShadowMap*> m_pShadowMapList;
};