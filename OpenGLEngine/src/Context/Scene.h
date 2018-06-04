#pragma once

#include "../Model/Model.h"
#include "../Light/Light.h"
#include "../Texture/Texture.h"
#include "../Buffer/UniformBufferMgr.h"
#include "../Shadow/ShadowMap.h"
#include "../Buffer/GBuffer.h"

#include "Setting.h"

class Scene
{
public:
	Scene(const Setting & setting);
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

    inline size_t GetModelSize()                  const { return m_pSceneModelList.size();      }
    inline Model* GetModelPtr(const UINT index)   const { return m_pSceneModelList[index];      }

    inline Camera* GetActivateCamera()            const { return m_pCameraList[m_activeCamera]; }

    inline UniformBufferMgr* GetUniformBufferMgr()      { return &m_uniformBufferMgr;           }
    inline GLuint GetMaterialUniformBufferIndex() const { return m_materialUniformBufferIndex;  }

    void setSceneShader(
        char* const vertexShaderFile,
        char* const fragmentShaderFile);

    inline void SetActiveCamera(const UINT activeCameraIndex) { m_activeCamera = activeCameraIndex; }

    inline void SetBackGroundColor(const Vector4& backgroundColor) {m_backgroundColor = backgroundColor; }

private:
    void drawScene();

	Setting setting;

    Vector4 m_backgroundColor;

    DirectionalLight m_directionalLight;
    PointLight       m_pointLight;

    std::vector<Camera*> m_pCameraList;
    UINT m_activeCamera;

	std::vector<Model*>   m_pSceneModelList;
	std::vector<Texture*> m_pTextureList;

    Shader m_sceneShader;
    Shader m_defferedRendingShader;

    // Uniform buffer and managers
    UniformBufferMgr m_uniformBufferMgr;

    GLuint m_transUniformbufferIndex;
    GLuint m_directionalLightUniformBufferIndex;
    GLuint m_pointLightUniformBufferIndex;
    GLuint m_materialUniformBufferIndex;

    GLint  success;
    GLchar compileLog[512];

    // Shadow map test
    void shadowPass();
    ShadowMap* m_pShadowMap;

    // Deferred shading
    void deferredDrawScene();
    GBuffer* m_pGBuffer;

    // Test
    GLuint m_directionalLightUniformBufferIndex2;
};