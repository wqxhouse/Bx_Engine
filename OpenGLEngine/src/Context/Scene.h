#pragma once

#include "../Model/Model.h"
#include "../Light/LightMgr.h"
#include "../Buffer/UniformBufferMgr.h"
#include "../Shadow/ShadowMap.h"
#include "../Buffer/GBuffer.h"
#include "../Shadow/SSAO.h"
#include "../Effect/Skybox.h"
#include "../Probe/LightProbe.h"

#include "Setting.h"

class Scene
{
public:
	Scene(Setting* pSetting);
    BOOL initialize();
	void update(float deltaTime);
        
    void preDraw();  // Pre-draw: shadow map, SSAO, etc
    void draw();
    void postDraw(); // Post draw: post processing, etc
    
    ~Scene();

	void addModel(
        const std::string& modelFile,
        const std::string& materialFile,
        Trans* modelTrans);

    inline void AddDirectionalLight(
        const Math::Vector3& direction,
        const Math::Vector3& color)
    {
        //m_pSceneLights.push_back(new DirectionalLight(direction, color));
        m_lightMgr.addDirectionalLight(direction, color);
    }

    inline void AddPointLight(
        const Math::Vector3& pos,
        const Math::Vector3& color,
        const float          radius)
    {
        //m_pSceneLights.push_back(new PointLight(pos, color, radius));
        m_lightMgr.addPointLight(pos, color, radius);
    }

    inline void AddSpotLight(
        const Math::Vector3& pos,
        const Math::Vector3& color,
        const float          innerRadius,
        const float          outerRadius)
    {
        //m_pSceneLights.push_back(new SpotLight(pos, color, innerRadius, outerRadius));
        m_lightMgr.addSpotLight(pos, color, innerRadius, outerRadius);
    }

    void addProspectiveCamera(
        const glm::vec3& pos,
        const glm::vec3& center,
        const glm::vec3& up,
        float speed,
        float aspectRatio,
        float nearClip = 0.1f,
        float farClip  = 100.0f,
        float fov      = 45.0f);

    void addOrthographicCamera(
        const glm::vec3& pos,
        const glm::vec3& center,
        const glm::vec3& up,
        const float      speed,
        const Rectangle  viewport,
        const float      nearClip = 0.1f,
        const float      farClip  = 100.0f);

    void addTexture(
        const std::string& textureFile,
        const GLenum       textureType = GL_TEXTURE_2D,
        const GLenum       format      = GL_RGBA,
        const GLenum       type        = GL_UNSIGNED_BYTE,
        const GLenum       wrapMethod  = GL_REPEAT,
        const BOOL         mipmap      = GL_FALSE);

    void addSkyboxImage(
        const char* const x_frontImg,
        const char* const x_backImg,
        const char* const y_frontImg,
        const char* const y_backImg,
        const char* const z_frontImg,
        const char* const z_backImg);

    void setGlobalMaterial(Material* pMaterial);
    void enableSceneLocalMaterial();
    void disableSceneLocalMaterial();

    inline Setting* GetSetting()                     const { return m_pSetting;                           }
                                                                                                         
    inline size_t GetModelSize()                     const { return m_pSceneModelList.size();             }
    inline Model* GetModelPtr(const UINT index)      const { return m_pSceneModelList[index];             }

    inline Camera* GetActivateCamera()               const { return m_pActiveCamera;                      }

    inline UniformBufferMgr* GetUniformBufferMgr()         { return &m_uniformBufferMgr;                  }
    inline GLuint GetMaterialUniformBufferIndex()    const { return m_materialUniformBufferIndex;         }
    inline GLuint GetPBRMaterialUniformBufferIndex() const { return m_pbrMaterialUniformBufferIndex;      }

    inline ShadowMap* GetShadowMap()                 const { return m_pShadowMap;                         }

    inline GBuffer*   GetGBuffer()                   const { return m_pGBuffer;                           }

    inline CookTorranceMaterial GetGlobalMaterial()  const { return m_globalPbrMaterial;                  }
                                                                                                          
    inline SSAO* GetSSAO()                           const { return m_pSsao;                              }
                                                                                                          
    inline Vector4 GetBackGroundColor()              const { return m_backgroundColor;                    }

    void setSceneShader(
        char* const vertexShaderFile,
        char* const fragmentShaderFile);

    inline void SetActiveCamera(Camera* pCam) { m_pActiveCamera = pCam; }
    inline void SetActiveCameraIndex(const UINT activeCameraIndex) { m_activeCameraIndex = activeCameraIndex; }

    inline void SetBackGroundColor(const Vector4& backgroundColor) {m_backgroundColor = backgroundColor; }
    
    BOOL useSSAO();
    inline void EnableSSAO() { m_pSetting->m_graphicsSetting.EnableSSAO(); }
    inline void DisableSSAO() { m_pSetting->m_graphicsSetting.DisableSSAO(); }

    inline void EnableRealtimeLightProbe() { enableRealtimeLightProbe = TRUE; }

private:
    BOOL initializePhongRendering();

    void drawScene();

	Setting* m_pSetting;

    GLint  success;
    GLchar compileLog[1024];

    Vector4 m_backgroundColor;
    
    // Shaders
    Shader m_sceneShader;
    Shader m_deferredRenderingShader;

    Shader m_pbrShader;

    // Scene objects (ex. Camera, model, texture, light)
    std::vector<Camera*> m_pCameraList;
    UINT m_activeCameraIndex;
    Camera* m_pActiveCamera;

    std::vector<Model*>   m_pSceneModelList;
    std::vector<Texture*> m_pTextureList;

    LightMgr m_lightMgr;

    // TODO: Remove these light (All lights are managed in light manager)

    // Scene lights
    struct SceneLights
    {
        UINT   count;
        Light* pLights;
    };

    std::vector<Light*> m_pSceneLights;
    //DirectionalLight m_directionalLight;

    PointLight m_pointLight;

    // Uniform buffer and managers
    UniformBufferMgr m_uniformBufferMgr;

    // UBO index
    GLuint m_resolutionUniformBufferIndex;
    GLuint m_transUniformbufferIndex;
    GLuint m_directionalLightUniformBufferIndex;
    GLuint m_pointLightUniformBufferIndex;
    GLuint m_materialUniformBufferIndex;
    GLuint m_pbrMaterialUniformBufferIndex;

    // Shadow map test
    BOOL initializeShadowMap();
    void shadowPass();
    ShadowMap* m_pShadowMap;

    // Deferred shading
    BOOL initializeDeferredRendering();
    void deferredDrawScene();
    GBuffer* m_pGBuffer;

    // PBR
    BOOL initializePBRendering();
    CookTorranceMaterial m_globalPbrMaterial;
    BOOL useGlobalMaterial;

    // SSAO
    SSAO* m_pSsao;

    // Skybox
    Skybox* m_pSkybox;
    std::vector<std::string> m_skyboxImages;

    // Light Probe
    LightProbe* m_pLightProbe;
    BOOL enableRealtimeLightProbe;

    friend void LightProbe::draw();
};
