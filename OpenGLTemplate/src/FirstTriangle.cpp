#include "../header/FirstTriangle.h"

FirstTriangle::FirstTriangle()
	:vertices
	{
		{ {  0.5f, -0.5f,  0.0f }, { 1.0f, 0.0f, 0.0f } },
		{ { -0.5f, -0.5f,  0.0f }, { 0.0f, 1.0f, 0.0f } },
		{ { 0.0f,  0.5f,  0.0f }, { 0.0f, 0.0f, 1.0f } }
	},
	indices{ 0, 1, 2 },
	m_shaderCompiler()
{
}

int FirstTriangle::Initialize()
{
	//Create vertex buffer and vertex array
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	//Set vertex buffer data
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Create index buffer data and bind to VAO
	glGenBuffers(1, &indexBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Bind vertex buffer attributes to VAO
	/*
		glVertexAttribPointer:
		1. index in vertex shader,
		2. counts of every element,
		3. elements type,
		4. if need to be normalized to 0-1,
		5. space between attribute sets,
		6. offset in every stride
	*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);


	//Compile shaders
	const char* vertexShaderFile = "SingleTriangle.vert";
	const char* fragmentShaderFile = "SingleTriangle.frag";
	int hs = m_shaderCompiler.compileShader(vertexShaderFile, fragmentShaderFile, &shaderProgram);
	/*int hs = m_shaderCompiler.compileShader("OpenGLTemplate/src/shaders", vertexShaderFile, "OpenGLTemplate/src/shaders", 
		fragmentShaderFile, &shaderProgram);*/

	if (hs != 0)
	{
		printf("Fail to compile shaders.\n");
		return -1;
	}

	return 0;
}

void FirstTriangle::Draw()
{
	glUseProgram(shaderProgram);
	glBindVertexArray(vertexArrayObject);
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//Wireframe mode
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

	//glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

FirstTriangle::~FirstTriangle()
{
	glDeleteBuffers(1, &vertexArrayObject);
	glDeleteBuffers(1, &indexBufferObject);
	glDeleteVertexArrays(1, &vertexBufferObject);
	glDeleteProgram(shaderProgram);
}