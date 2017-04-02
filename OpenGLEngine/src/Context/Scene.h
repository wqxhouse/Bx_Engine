#pragma once

#include "../Compiler/ShaderCompiler.h"
#include "../Model/Mesh/Mesh.h"
#include "../Camera/Camera.h"
#include "Setting.h"

class Scene
{
public:
	Scene(const Setting & setting);
	int initialize();
	void update(float deltaTime);
	void draw();
	~Scene();

	ProspectiveCamera m_proj_camera;
private:

	GLuint vertexArrayObject;
	GLuint vertexBufferObject;

	GLuint indexBufferObject;

	GLuint textureBinder;

	GLuint shaderProgram;

	GLint success;
	GLchar compileLog[512];

	Setting setting;

	Mesh *mesh;
};