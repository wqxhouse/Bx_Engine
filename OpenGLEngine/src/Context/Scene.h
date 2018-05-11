#pragma once

#include "../Compiler/ShaderCompiler.h"
#include "../Model/Model.h"
#include "../Camera/Camera.h"
#include "../Light/Light.h"
#include "../Texture/Texture.h"
#include "../Buffer/UniformBuffer.h"
#include "Setting.h"

struct SimpleDirectionalLight
{
    Vector4 lightDir;
    Vector4 ligthColor;
};

class Scene
{
public:
	Scene(const Setting & setting);
	int initialize();
	void update(float deltaTime);
	void draw();
	~Scene();

	void addModel(const std::string & modelFile, const std::string & materialFile, Transform* modelTrans);

	std::vector<Camera*> m_pCameraList;
	UINT m_activeCamera;

	DirectionalLight m_directionalLight;
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

    UniformBlockMemberData m_lightData[2];
    UniformBuffer* m_lightBuffer;

    UniformBlockMemberData m_lightData2[2];
    UniformBuffer* m_lightBuffer2;

    UniformBlockMemberData m_materialData[4];
    UniformBuffer* m_materialBuffer;

	std::vector<Model*> m_pSceneModelList;
	std::vector<Texture*> m_pTextureList;
};