#pragma once

#include "../header/OpenGLPCH.h"

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

	//TODO: Build the compiler 
	void getStringFromFile(const char* path, char** resStr);

	//Test
	int compileShaders();
};