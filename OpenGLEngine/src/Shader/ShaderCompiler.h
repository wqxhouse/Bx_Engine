#pragma once

#include "../Core/OpenGLPCH.h"

#define DEFAULT_SHADER_SIZE 65536

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
        const unsigned int vertexShaderSourceSize = DEFAULT_SHADER_SIZE,
        const unsigned int fragShaderSourceSize   = DEFAULT_SHADER_SIZE);

	BOOL compileShader(
        const char*  const vertexShaderFile,
        const char*  const fragmentShaderFile,
        const char*  const geometryShaderFile,
        const char*  const tcsShaderFile,
        const char*  const tesShaderFile,
        OUT GLuint*  shaderProgram,
        const unsigned int vertexShaderSourceSize = DEFAULT_SHADER_SIZE,
        const unsigned int fragShaderSourceSize   = DEFAULT_SHADER_SIZE);

private:
	char* shaderPath;

	// Parsing the shader file
	void combileShaderPathAndFile(
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