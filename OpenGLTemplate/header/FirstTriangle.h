#pragma once

#include <OpenGLPCH.h>
#include <ShaderCompiler.h>

using namespace std;

class FirstTriangle
{
public:
	FirstTriangle();
	int Initialize();
	void Draw();
	~FirstTriangle();
private:
	struct Vertex
	{
		GLfloat pos[3];
		GLfloat color[3];
	};

	GLuint vertexArrayObject;
	GLuint vertexBufferObject;

	GLuint indexBufferObject;

	GLuint shaderProgram;
	GLuint vertexShader;
	GLuint fragShader;

	Vertex vertices[12];
	GLuint indices[6];

	GLint success;
	GLchar compileLog[512];

	ShaderCompiler m_shaderCompiler;
};