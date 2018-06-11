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
    BOOL compileShader(
        const char*  vertexShaderPath,
        const char*  vertexShaderFile,
		const char*  fragmentShaderPath,
        const char*  fragmentShaderFile,
        OUT GLuint*  shaderProgram,
		unsigned int vertexShaderSourceSize = 8192,
        unsigned int fragShaderSourceSize   = 8192);

	BOOL compileShader(
        const char*  vertexShaderFile,
        const char*  fragmentShaderFile,
        OUT GLuint*  shaderProgram,
		unsigned int vertexShaderSourceSize = 8192,
        unsigned int fragShaderSourceSize   = 8192);

private:
	char* shaderPath;

	// Parsing the shader file
	void ShaderCompiler::combileShaderPathAndFile(
        const char*     path,
        const char*     file,
        OUT const char* filePath);

    void writeToShaderSource(
        FILE*       pFile,
        const UINT  sourceSize,
        UINT*       writtenSize,
        OUT char*   shaderSource);

    // Preprocessing macros in shader
    void preprocessingShaderFile(
        const char* line,
        UINT        sourceSize,
        UINT*       writtenSize,
        OUT char*   shaderSource);

	void parseShaderFile(
        const char* file,
        UINT        sourceSize,
        OUT char*   shaderSource);

	void parseShaderFile(
        const char* path,
        const char* file,
        UINT        sourceSize,
        OUT char*   shaderSource);
};