#include "ShaderCompiler.h"

ShaderCompiler::ShaderCompiler()
{
	this->shaderPath = "../Resources/shaders/";
}

ShaderCompiler::ShaderCompiler(const char * shaderPath)
{
	memcpy(this->shaderPath, shaderPath, strlen(shaderPath) + 1);
}

const char* ShaderCompiler::combileShaderPathAndFile(const char * path, const char * file)
{
	char* filePath = (char*)malloc(strlen(path) + strlen(file) + 1);
	strcpy_s(filePath, strlen(path), shaderPath);
	strcat_s(filePath, strlen(file) + 1, file);
	return (const char*)filePath;
}


void ShaderCompiler::parseShaderFile(const char * path, const char * file, unsigned int sourceSize, OUT char * shaderSource)
{
	const char* filePath = combileShaderPathAndFile(path, file);
	parseShaderFile(filePath, sourceSize, shaderSource);
	free((char*)filePath);
}


void ShaderCompiler::parseShaderFile(const char * file, unsigned int sourceSize, OUT char * shaderSource)
{
	const char* filePath = combileShaderPathAndFile(shaderPath, file);
	FILE* filePtr = nullptr;
	fopen_s(&filePtr, filePath, "r");
	char fileCharacter;
	unsigned int size = 0;

	while ((fileCharacter = getc(filePtr)) != EOF)
	{
		size += sizeof(fileCharacter);

		if (size < sourceSize)
		{
			strcat_s(shaderSource, sizeof(char), &fileCharacter);
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
	}
	strcat_s(shaderSource, sizeof(char), '\0');
}


void ShaderCompiler::setDefaultShaderPath(const char* path)
{
	memcpy(this->shaderPath, path, strlen(path) + 1);
}

char* ShaderCompiler::getDefaultPath()
{
	return shaderPath;
}


int ShaderCompiler::compileShader(const char * vertexShaderPath, const char * vertexShaderFile, 
	const char * fragmentShaderPath, const char * fragmentShaderFile, OUT GLuint* shaderProgram,
	unsigned int vertexShaderSourceSize, unsigned int fragmentShaderSourceSize)
{
	const char* vertexShaderFilePath = combileShaderPathAndFile(vertexShaderPath, vertexShaderFile);
	const char* fragmentShaderFilePath = combileShaderPathAndFile(fragmentShaderPath, fragmentShaderFile);
	
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

	//Comile fragment(pixal) shader
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
		cerr << "Fail to compile fragment shader.\n" << compileLog << endl;
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
		cerr << "Fail to link shaders in the program.\n" << compileLog << endl;

		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(*shaderProgram, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
		return -1;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);
}


ShaderCompiler::~ShaderCompiler()
{

}
