#pragma once

#include "../Compiler/ShaderCompiler.h"
#include "../Model/Model.h"
#include "../Camera/Camera.h"
#include "../Light/Light.h"
#include "Setting.h"

class Scene
{
public:
	Scene(const Setting & setting);
	int initialize();
	void update(float deltaTime);
	void draw();
	~Scene();

	void addModel(const std::string & modelFile, Transform* modelTrans);

	ProspectiveCamera m_proj_camera;

	DirectionalLight m_directionalLight;
private:

	GLuint vertexArrayObject[2];
	
	GLuint vertexBufferObject[2];

	GLuint indexBufferObject[2];

	GLuint textureHandle[2];

	GLuint shaderProgram;

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
	
	GLuint lightParagHandle;
	GLuint lightParagBuffer;
	const GLchar* lightMembers[2] = { "lightDir", "lightColor" };
	GLint lightStructSize;
	GLubyte* lightDataBuffer;

	//Mesh* mesh;
	//Model* pModel;
	std::vector<Model*> sceneModelsPtr;
};