#pragma once

#include "../Buffer/Framebuffer.h"
#include "../Shader/GraphicsShader.h"
#include "../Camera/Camera.h"
#include "../Context/Setting.h"

class Light;
class Scene;

class ShadowMap
{
public:
    ShadowMap(
        Scene* pScene,
        Light* pLight); ///< The light be casted shadow

    ShadowMap(
        Scene*     pScene,
        Camera*    pCam,
        Resolution shadowResolution); ///< Directly set the camera to cast shadow

    ~ShadowMap();

    BOOL initialize();
    void update(Light* pLight);
    void drawShadowMap(Scene* pScene);
    /*void readShadowMap(
        const GLenum       texUnit,
        const GLuint       shaderProgram,
        const std::string& samplerName,
        const UINT         samplerIndex);*/

    inline glm::mat4 GetLightTransVP()
    {
        glm::mat4 result;

        if (m_pLightCamera != NULL)
        {
            result = ToGLMMat4(m_pLightCamera->GetProjectionMatrix()) *
                     ToGLMMat4(m_pLightCamera->GetViewMatrix());
        }

        return result;
    }

    inline GLuint GetShadowResolutionUniformBufferIndex() const
    {
        return m_shadowResolutionUniformBufferIndex;
    }

    inline Camera* GetLightCamera() const { return m_pLightCamera; }

private:
    void initializeLightCamera();

    // Framebuffer m_shadowMapFramebuffer;
    GraphicsShader m_shadowMapShader;

    Scene*  m_pScene;
    Light*  m_pLight;
    Camera* m_pLightCamera;

    GLuint     m_shadowResolutionUniformBufferIndex; // Shadow resolution UBO
    Resolution m_shadowResolution;                   // Shadow resolution data block

    UINT m_shadowMapSamples;
};