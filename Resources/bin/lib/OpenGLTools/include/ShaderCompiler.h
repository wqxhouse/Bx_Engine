#pragma once

#define IN
#define OUT

//Include GLEW Library
#define GLEW_STATIC
#include <GL/glew.h>

//Include GLFW Library
#include <GLFW/glfw3.h>


class ShaderCompiler
{
public:
	ShaderCompiler();
	ShaderCompiler(const char* shaderPath);

	void setDefaultShaderPath(const char* path);
	char* getDefaultPath();

	//Compile vertex and fragment shader, then link them
	int compileShader(const char* vertexShaderPath, const char* vertexShaderFile,
		const char* fragmentShaderPath, const char* fragmentShaderFile, OUT GLuint* shaderProgram,
		unsigned int vertexShaderSourceSize = 1024, unsigned int fragShaderSourceSize = 1024);
	int compileShader(const char* vertexShaderFile, const char* fragmentShaderFile, OUT GLuint* shaderProgram,
		unsigned int vertexShaderSourceSize = 1024, unsigned int fragShaderSourceSize = 1024);
private:
	char* shaderPath;

	//Parsing the shader file
	const char* combileShaderPathAndFile(const char* path, const char* file);
	void parseShaderFile(const char* file, unsigned int sourceSize, OUT char* shaderSource);
	void parseShaderFile(const char* path, const char* file, unsigned int sourceSize, OUT char* shaderSource);
};