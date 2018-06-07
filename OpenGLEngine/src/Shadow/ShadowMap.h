#pragma once

#include "../Buffer/Framebuffer.h"
#include "../Shader/Shader.h"
#include "../Camera/Camera.h"

class Light;
class Scene;

class ShadowMap
{
public:
    ShadowMap(
        Scene*     pScene,
        Light*     pLight,         ///< The light be casted shadow
        const UINT depthTexWidth,
        const UINT depthTexHeight,
        const UINT samples);
    ~ShadowMap();

    BOOL initialize();
    void update(Light* pLight);
    void drawShadowMap(Scene* pScene);
    void readShadowMap(
        const GLenum       texUnit,
        const GLuint       shaderProgram,
        const std::string& samplerName,
        const UINT         samplerIndex);

    inline glm::mat4 GetLightTransVP()
    {
        return m_pLightCamera->GetProjectionMatrix() *
               m_pLightCamera->GetViewMatrix();
    }

    inline GLuint GetShadowResolutionUniformBufferIndex() const
    {
        return m_shadowResolutionUniformBufferIndex;
    }

private:
    void initializeLightCamera();

    Framebuffer m_shadowMapFramebuffer;
    Shader      m_shadowMapShader;

    GLuint m_shadowResolutionUniformBufferIndex;
    UINT   m_sceneWidth;
    UINT   m_sceneHeight;

    Scene*  m_pScene;
    Light*  m_pLight;
    Camera* m_pLightCamera;

    UINT m_shadowMapWidth;
    UINT m_shadowMapHeight;

    UINT m_shadowMapSamples;
};