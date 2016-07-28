#pragma once

#include "../header/OpenGLPCH.h"
#include <ShaderCompiler.h>

class FirstTriangle
{
public:
	FirstTriangle();
	int Initialize();
	void Render();
	~FirstTriangle();
private:
	GLuint VAO;//Vertex Array Object
	GLuint VBO;//Vertex Buffer Object

	GLuint shaderProgram;
	GLuint vertexShader;
	GLuint fragShader;

	//GLuint vertexbuffer;
	GLfloat vertices[9];

	GLint success;
	GLchar compileLog[512];

	ShaderCompiler m_shaderCompiler;
};