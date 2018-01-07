#pragma once

#include "../Core/OpenGLPCH.h"

class ShaderCompiler
{
public:
	ShaderCompiler();
	ShaderCompiler(const char* shaderPath);

	~ShaderCompiler();

	void setDefaultShaderPath(const char* path);
	const char* getDefaultPath();

	//Compile vertex and fragment shader, then link them
	int compileShader(const char* vertexShaderPath, const char* vertexShaderFile,
		const char* fragmentShaderPath, const char* fragmentShaderFile, OUT GLuint* shaderProgram,
		unsigned int vertexShaderSourceSize = 2048, unsigned int fragShaderSourceSize = 2048);
	int compileShader(const char* vertexShaderFile, const char* fragmentShaderFile, OUT GLuint* shaderProgram,
		unsigned int vertexShaderSourceSize = 2048, unsigned int fragShaderSourceSize = 2048);
private:
	char* shaderPath;

	//Parsing the shader file
	void ShaderCompiler::combileShaderPathAndFile(const char * path, const char * file, OUT const char* filePath);
	void parseShaderFile(const char* file, unsigned int sourceSize, OUT char* shaderSource);
	void parseShaderFile(const char* path, const char* file, unsigned int sourceSize, OUT char* shaderSource);
};