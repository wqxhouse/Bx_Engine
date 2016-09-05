#pragma once

#include "../header/OpenGLPCH.h"
#include <ShaderCompiler.h>

class FirstTriangle
{
public:
	FirstTriangle();
	int Initialize();
	void Draw();
	~FirstTriangle();
private:
	GLuint vertexArrayObject;
	GLuint vertexBufferObject;

	GLuint indexBufferObject;

	GLuint shaderProgram;
	GLuint vertexShader;
	GLuint fragShader;

	GLfloat vertices[12];
	GLuint indices[6];

	GLint success;
	GLchar compileLog[512];

	ShaderCompiler m_shaderCompiler;
};