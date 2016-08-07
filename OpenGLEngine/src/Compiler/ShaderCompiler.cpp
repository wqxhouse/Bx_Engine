#include "../../include/ShaderCompiler.h"

ShaderCompiler::ShaderCompiler()
{
	shaderPath = (char*)malloc(512 * sizeof(char));
#if _DEBUG
	char defaultPath[] = "../Resources/shaders/\0";
#else
	this->shaderPath = "../../Resources/shaders/\0";
#endif
	memcpy(this->shaderPath, defaultPath, sizeof(defaultPath));
}

ShaderCompiler::ShaderCompiler(const char * shaderPath)
{
	memcpy((char*)(this->shaderPath), shaderPath, strlen(shaderPath) + 1);
}

void ShaderCompiler::combileShaderPathAndFile(const char * path, const char * file, OUT const char* filePath)
{
	//memset((char*)filePath, 0, strlen(filePath));
	strcpy((char*)filePath, path);
	strcat((char*)filePath, "/");
	strcat((char*)filePath, file);
	strcat((char*)filePath, "\0");
}


//void ShaderCompiler::parseShaderFile(const char * path, const char * file, unsigned int sourceSize, OUT char * shaderSource)
//{
//	const char* filePath = nullptr;
//	memcpy((char*)(this->shaderPath), path, strlen(path));
//	parseShaderFile(filePath, sourceSize, shaderSource);
//}


void ShaderCompiler::parseShaderFile(const char * file, unsigned int sourceSize, OUT char * shaderSource)
{
	const char* filePath = (const char*)malloc(256 * sizeof(char));
	combileShaderPathAndFile(shaderPath, file, filePath);
	FILE* filePtr = nullptr;
	fopen_s(&filePtr, filePath, "r");
	char fileCharacter;
	unsigned int size = 0;

	if (filePtr != NULL)
	{
		memset(shaderSource, 0, sourceSize);
		while ((fileCharacter = getc(filePtr)) != EOF)
		{
			if (size < sourceSize)
			{
				if (size == 0)
				{
					strncpy(shaderSource, &fileCharacter, 1);
				}
				else
				{
					strncat(shaderSource, &fileCharacter, 1);
				}
			}
			else
			{
				printf("Exceed the shader source size, reallocate memory position.");
				sourceSize *= 2;
				char* newShaderSource = (char*)realloc(shaderSource, sourceSize);

				if (newShaderSource != nullptr)
				{
					free(shaderSource);
					shaderSource = newShaderSource;
				}
				else
				{
					free(shaderSource);
					printf("Error when reallocate for shader source.\n");
					exit(1);
				}
			}

			size += sizeof(fileCharacter);
		}
		char endChar = '\0';
		strncat(shaderSource, &endChar, 1);
	}
	else
	{
		printf("Can't open the shader file: %s\n", file);
	}

	//Release file path array
	SafeRelease((void*)filePath, MALLOC);
}


void ShaderCompiler::setDefaultShaderPath(const char* path)
{
	memset(this->shaderPath, 0, strlen(shaderPath));
	memcpy(this->shaderPath, path, strlen(path));
	strcat(this->shaderPath, "\0");
	//memcpy((char*)(this->shaderPath), path, strlen(path) + 1);
}

const char* ShaderCompiler::getDefaultPath()
{
	return shaderPath;
}


//TODO: Crash when using customized shader file path
int ShaderCompiler::compileShader(const char * vertexShaderPath, const char * vertexShaderFile, 
	const char * fragmentShaderPath, const char * fragmentShaderFile, OUT GLuint* shaderProgram,
	unsigned int vertexShaderSourceSize, unsigned int fragmentShaderSourceSize)
{
	const char* vertexShaderFilePath = (const char*)malloc(256 * sizeof(char));
	combileShaderPathAndFile(vertexShaderPath, vertexShaderFile, vertexShaderFilePath);
	const char* fragmentShaderFilePath = (const char*)malloc(256 * sizeof(char));
	combileShaderPathAndFile(fragmentShaderPath, fragmentShaderFile, fragmentShaderFilePath);
	setDefaultShaderPath(vertexShaderPath);
	
	int hs = compileShader(vertexShaderFilePath, fragmentShaderFilePath, shaderProgram, 
		vertexShaderSourceSize, fragmentShaderSourceSize);

	return hs;
}


int ShaderCompiler::compileShader(const char * vertexShaderFile, const char * fragmentShaderFile, 
	OUT GLuint* shaderProgram, unsigned int vertexShaderSourceSize, unsigned int fragmentShaderSourceSize)
{
	GLint success = true;//Indicator of compile result
	char compileLog[512];

	//Compile vertex shader
	char* vertexShaderSource = (char*)malloc(vertexShaderSourceSize * sizeof(char));
	parseShaderFile(vertexShaderFile, vertexShaderSourceSize, vertexShaderSource);

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (success)
	{
		printf("Vertex shader is successly compiled.\n");
	}
	else
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, compileLog);
		printf("Fail to compile vertex shader.\n%s\n", compileLog);
		return -1;
	}

	//Compile fragment(pixal) shader
	char* fragmentShaderSource = (char*)malloc(vertexShaderSourceSize * sizeof(char));
	parseShaderFile(fragmentShaderFile, fragmentShaderSourceSize, fragmentShaderSource);

	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragShader);

	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
	if (success)
	{
		printf("Fragment shader is successly compiled.\n");
	}
	else
	{
		glGetShaderInfoLog(fragShader, 512, NULL, compileLog);
		printf("Fail to compile fragment shader.\n%s\n", compileLog);
		return -1;
	}

	*shaderProgram = glCreateProgram();	
	glAttachShader(*shaderProgram, vertexShader);
	glAttachShader(*shaderProgram, fragShader);
	glLinkProgram(*shaderProgram);

	int InfoLogLength;
	glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
	glGetProgramiv(*shaderProgram, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if (success)
	{
		printf("Shader program is successly linked.\n");
	}
	else
	{
		glGetShaderInfoLog(*shaderProgram, InfoLogLength, NULL, &compileLog[0]);
		printf("Fail to link shaders in the program.\n");

		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(*shaderProgram, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
		return -1;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);

	SafeRelease(vertexShaderSource, MALLOC);
	SafeRelease(fragmentShaderSource, MALLOC);

	return 0;
}


ShaderCompiler::~ShaderCompiler()
{
	SafeRelease((char*)shaderPath, MALLOC);
}
