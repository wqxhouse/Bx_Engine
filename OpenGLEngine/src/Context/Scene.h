#pragma once

#include "../Compiler/ShaderCompiler.h"
#include "../Model/Model.h"
#include "../Camera/Camera.h"
#include "../Light/Light.h"
#include "../Texture/Texture.h"
#include "../Buffer/UniformBufferMgr.h"
#include "Setting.h"

class Scene
{
public:
	Scene(const Setting & setting);
    BOOL initialize();
	void update(float deltaTime);
	void draw();
	~Scene();

	void addModel(const std::string & modelFile, const std::string & materialFile, Transform* modelTrans);
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

    inline void SetActiveCamera(const UINT activeCameraIndex) { m_activeCamera = activeCameraIndex; }

private:
	GLuint simpleTextureProgram;

	GLint success;
	GLchar compileLog[512];

	Setting setting;

	GLuint transformHandle;
	GLuint transformBuffer;
	const GLchar* transformMembers[3] = { "world", "view", "proj" };
	GLint transformBufferSize;
	GLubyte* transformBufferData;

	GLuint normalTransformHandle;
	GLuint normalTransformBuffer;
	const GLchar* normalTransformMembers = { "rot" };
	GLint normalTransformBufferSize;
	GLubyte* normalTransformBufferData;

    // Uniform buffer and managers
    UniformBufferMgr m_uniformBufferMgr;

    GLuint m_transUniformbufferIndex;
    GLuint m_lightUniformBufferIndex;
    GLuint m_materialBufferIndex;

    UINT m_activeCamera;

    DirectionalLight m_directionalLight;

    std::vector<Camera*> m_pCameraList;
	std::vector<Model*> m_pSceneModelList;
	std::vector<Texture*> m_pTextureList;
};