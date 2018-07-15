#pragma once

#include "../Model/Model.h"
#include "../Light/Light.h"
#include "../Buffer/UniformBufferMgr.h"
#include "../Shadow/ShadowMap.h"
#include "../Buffer/GBuffer.h"
#include "../Shadow/SSAO.h"
#include "../Effect/Skybox.h"

#include "Setting.h"

class Scene
{
public:
	Scene(Setting* pSetting);
    BOOL initialize();
	void update(float deltaTime);
	void draw();
	~Scene();

	void addModel(
        const std::string& modelFile,
        const std::string& materialFile,
        Trans* modelTrans);

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

    inline Setting* GetSetting()                     const { return m_pSetting;                      }

    inline size_t GetModelSize()                     const { return m_pSceneModelList.size();        }
    inline Model* GetModelPtr(const UINT index)      const { return m_pSceneModelList[index];        }

    inline Camera* GetActivateCamera()               const { return m_pCameraList[m_activeCamera];   }

    inline UniformBufferMgr* GetUniformBufferMgr()         { return &m_uniformBufferMgr;             }
    inline GLuint GetMaterialUniformBufferIndex()    const { return m_materialUniformBufferIndex;    }
    inline GLuint GetPBRMaterialUniformBufferIndex() const { return m_pbrMaterialUniformBufferIndex; }

    inline ShadowMap* GetShadowMap()                 const { return m_pShadowMap;                    }

    inline GBuffer*   GetGBuffer()                   const { return m_pGBuffer;                      }

    inline CookTorranceMaterial GetGlobalMaterial()  const { return m_globalPbrMaterial;             }

    inline SSAO* GetSSAO()                           const { return m_pSsao;                         }

    void setSceneShader(
        char* const vertexShaderFile,
        char* const fragmentShaderFile);

    inline void SetActiveCamera(const UINT activeCameraIndex) { m_activeCamera = activeCameraIndex; }

    inline void SetBackGroundColor(const Vector4& backgroundColor) {m_backgroundColor = backgroundColor; }
    
    BOOL useSSAO();
    inline void EnableSSAO() { m_pSetting->m_graphicsSetting.EnableSSAO(); }
    inline void DisableSSAO() { m_pSetting->m_graphicsSetting.DisableSSAO(); }

    DirectionalLight m_directionalLight;

private:
    BOOL initializePhongRendering();
    void drawScene();

	Setting* m_pSetting;

    Vector4 m_backgroundColor;

    PointLight m_pointLight;

    std::vector<Camera*> m_pCameraList;
    UINT m_activeCamera;

	std::vector<Model*>   m_pSceneModelList;
	std::vector<Texture*> m_pTextureList;

    Shader m_sceneShader;
    Shader m_deferredRendingShader;

    Shader m_pbrShader;

    // Uniform buffer and managers
    UniformBufferMgr m_uniformBufferMgr;

    // UBO index
    GLuint m_transUniformbufferIndex;
    GLuint m_directionalLightUniformBufferIndex;
    GLuint m_pointLightUniformBufferIndex;
    GLuint m_materialUniformBufferIndex;
    GLuint m_pbrMaterialUniformBufferIndex;

    GLint  success;
    GLchar compileLog[512];

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
};
