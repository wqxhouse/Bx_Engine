#pragma once

#include "../Shader/Shader.h"
#include "../Model/Model.h"
#include "../Camera/Camera.h"
#include "../Light/Light.h"
#include "../Texture/Texture.h"
#include "../Buffer/UniformBufferMgr.h"
#include "../Buffer/Framebuffer.h"

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
        Transform* modelTrans);

    void addCamera(
        const CameraType type,
        const glm::vec3& pos,
        const glm::vec3& center,
        const glm::vec3& up,
        float speed,
        float aspectRatio,
        float nearClip = 0.1f,
        float farClip  = 100.0f,
        float fov      = 45.0f);

    void setSceneShader(
        char* const vertexShaderFile,
        char* const fragmentShaderFile);

    inline void SetActiveCamera(const UINT activeCameraIndex) { m_activeCamera = activeCameraIndex; }

    inline void SetBackGroundColor(const Vector4& backgroundColor)
    {
        m_backgroundColor = backgroundColor;
    }

private:
    void shadowPass();

    void drawPass();

	Setting setting;

    Vector4 m_backgroundColor;

    DirectionalLight m_directionalLight;
    PointLight       m_pointLight;

    std::vector<Camera*> m_pCameraList;
    UINT m_activeCamera;

    ProspectiveCamera* m_pLightCamera;

	std::vector<Model*> m_pSceneModelList;
	std::vector<Texture*> m_pTextureList;

    Shader m_sceneShader;

    // Uniform buffer and managers
    UniformBufferMgr m_uniformBufferMgr;

    GLuint m_transUniformbufferIndex;
    GLuint m_directionalLightUniformBufferIndex;
    GLuint m_pointLightUniformBufferIndex;
    GLuint m_materialBufferIndex;

    GLint success;
    GLchar compileLog[512];

    // Shadow map test
    Shader m_shadowMapShader;
    DepthFramebuffer m_shadowMap;

};