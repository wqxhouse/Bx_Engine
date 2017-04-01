#pragma once

#include "Core/OpenGLPCH.h"
#include "Compiler/ShaderCompiler.h"
#include "Mesh/Model/Mesh.h"
#include "Camera/Camera.h"

using namespace std;

class FirstTriangle
{
public:
	FirstTriangle();
	int Initialize();
	void update(float deltaTime);
	void draw();
	~FirstTriangle();

	ProspectiveCamera m_proj_camera;
private:

	GLuint vertexArrayObject;
	GLuint vertexBufferObject;

	GLuint indexBufferObject;

	GLuint shaderProgram;
	GLuint vertexShader;
	GLuint fragShader;

	GLint success;
	GLchar compileLog[512];

	Mesh *mesh;

	/*
	struct Vertex
	{
		GLfloat pos[3];
		GLfloat color[3];
		GLfloat texCoord[2];
	};
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	vertices
	{
	{ { -1.0f, -1.0f, -1.0f },{ 0.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },
	{ { -1.0f,  1.0f, -1.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
	{ { 1.0f,  1.0f, -1.0f },{ 1.0f, 1.0f, 0.0f },{ 1.0f, 1.0f } },
	{ { 1.0f, -1.0f, -1.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f } },
	{ { -1.0f, -1.0f,  1.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 0.0f } },
	{ { -1.0f,  1.0f,  1.0f },{ 0.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } },
	{ { 1.0f,  1.0f,  1.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f } },
	{ { 1.0f, -1.0f,  1.0f },{ 1.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } }
	},
	indices
	{
	0, 1, 2, 0, 2, 3,
	4, 6, 5, 4, 7, 6,
	4, 5, 1, 4, 1, 0,
	3, 2, 6, 3, 6, 7,
	1, 5, 6, 1, 6, 2,
	4, 0, 3, 4, 3, 7
	},*/
};