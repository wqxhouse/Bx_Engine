#include "../header/FirstTriangle.h"

FirstTriangle::FirstTriangle()
	:vertices{ -0.5f, -0.5f, 0.0f,
				0.5f, -0.5f, 0.0f,
				0.0f,  0.5f, 0.0f }
{
}

int FirstTriangle::Initialize()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	/*
		glVertexAttribPointer:
		1. index in vertex shader,
		2. counts of every element,
		3. elements type,
		4. if need to be normalized to 0-1,
		5. space between attribute sets,
		6. offset in every stride
	*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	if (compileShaders() != 0)
	{
		cerr << "Fail to compile shaders.\n" << endl;
		return -1;
	}

	return 0;
}

void FirstTriangle::Render()
{
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}

FirstTriangle::~FirstTriangle()
{
	glDeleteBuffers(1, &VAO);
	glDeleteVertexArrays(1, &VBO);
	glDeleteProgram(shaderProgram);
}

void FirstTriangle::getStringFromFile(const char * path, char** resStr)
{
	string shaderStr;

	ifstream shaderFile(path);
	if (shaderFile.is_open())
	{
		string singleShaderline;
		while (getline(shaderFile, singleShaderline))
		{
			shaderStr += singleShaderline + "\n";
		}
		shaderFile.close();
	}

	char* shaderCStr = (char*)shaderStr.c_str();
	*resStr = (char*)malloc(strlen(shaderCStr) + 1);
	memcpy(*resStr, shaderCStr, strlen(shaderCStr) + 1);
	//*resStr = (char*)shaderStr.c_str();// +"\0";
	//*resStr = shaderStr.c_str();
}

int FirstTriangle::compileShaders()
{
	const char* vertexPath = "../Resources/shaders/SingleTriangleVS.glsl";
	GLchar* vertexShaderSource = nullptr;
	getStringFromFile(vertexPath, &vertexShaderSource);

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
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
		cerr << "Fail to compile vertex shader.\n" << compileLog << endl;
		return -1;
	}

	const char* fragPath = "../Resources/shaders/SingleTriangleFS.glsl";
	GLchar* fragmentShaderSource = nullptr;
	getStringFromFile(fragPath, &fragmentShaderSource);

	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
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

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragShader);
	glLinkProgram(shaderProgram);

	int InfoLogLength;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if (success)
	{
		printf("Shader program is successly linked.\n");
	}
	else
	{
		glGetShaderInfoLog(shaderProgram, InfoLogLength, NULL, &compileLog[0]);
		cerr << "Fail to link shaders in the program.\n" << compileLog << endl;

		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(shaderProgram, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
		return -1;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);

	return 0;
}
