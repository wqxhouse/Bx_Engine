#include "../header/FirstTriangle.h"

FirstTriangle::FirstTriangle()
	:vertices{ -0.5f, -0.5f, 0.0f,
				0.5f, -0.5f, 0.0f,
				0.0f,  0.5f, 0.0f },
	m_shaderCompiler()
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

	//Compile shaders
	const char* vertexShaderFile = "SingleTriangleVS.glsl";
	const char* fragmentShaderFile = "SingleTriangleFS.glsl";
	int hs = m_shaderCompiler.compileShader(vertexShaderFile, fragmentShaderFile, &shaderProgram);

	if (hs != 0)
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