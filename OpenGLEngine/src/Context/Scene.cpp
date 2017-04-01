#include "Scene.h"
#include "../Model/Mesh/ObjModelLoader.h"

Scene::Scene(const Setting & setting)
	:m_proj_camera(glm::vec3(-5, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), 5.0f, (float)setting.width / (float)setting.height)
{
	this->setting = setting;
}

int Scene::initialize()
{
	//Load Model
	ObjModelLoader objLoader;
	objLoader.LoadModel("../Resources/models/sphere/sphere.obj", &mesh);

	//Create vertex buffer and vertex array
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	//Set vertex buffer data
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	//glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertexBuffer.size() * sizeof(Mesh::Vertex), mesh->vertexBuffer.data(), GL_STATIC_DRAW);

	//Create index buffer data and bind to VAO
	glGenBuffers(1, &indexBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer.size() * sizeof(GLuint), mesh->indexBuffer.data(), GL_STATIC_DRAW);

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (GLvoid*)(offsetof(Mesh::Vertex, Mesh::Vertex::position)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (GLvoid*)(offsetof(Mesh::Vertex, Mesh::Vertex::normal)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (GLvoid*)(offsetof(Mesh::Vertex, Mesh::Vertex::texCoords)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//Compile shaders
	ShaderCompiler m_shaderCompiler;
	const char* vertexShaderFile = "SingleTriangle.vert";
	const char* fragmentShaderFile = "SingleTriangle.frag";
	int hs = m_shaderCompiler.compileShader(vertexShaderFile, fragmentShaderFile, &shaderProgram);

	if (hs != 0)
	{
		printf("Fail to compile shaders.\n");
		return -1;
	}

	return 0;
}

void Scene::update(float deltaTime)
{
	m_proj_camera.update(deltaTime);
}

void Scene::draw()
{
	glUseProgram(shaderProgram);

	GLfloat timeValue = (GLfloat)glfwGetTime();
	GLfloat colorValue = 1.0f;// (GLfloat)(sin(timeValue) / 2) + 0.5;
	GLint glVertexColorLocation = glGetUniformLocation(shaderProgram, "uniformColor");
	glUniform3f(glVertexColorLocation, colorValue, colorValue, colorValue);

	glm::mat4 transform;
	transform = glm::rotate(transform, glm::radians(180.0f) * timeValue, glm::vec3(0, 1, 1));
	transform = glm::scale(transform, glm::vec3(.5f, .5f, .5f));

	GLint glWorldMatrixLocation = glGetUniformLocation(shaderProgram, "world");
	glUniformMatrix4fv(glWorldMatrixLocation, 1, GL_FALSE, glm::value_ptr(transform));

	GLint glViewMatrixLocation = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(glViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(m_proj_camera.getViewMatrix()));

	GLint glProjectionLocation = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(glProjectionLocation, 1, GL_FALSE, glm::value_ptr(m_proj_camera.getProjectionMatrix()));

	glBindVertexArray(vertexArrayObject);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//Wireframe mode
	//glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glDrawElements(GL_TRIANGLES, mesh->indexBuffer.size(), GL_UNSIGNED_INT, 0);

	//glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

Scene::~Scene()
{
	glDeleteBuffers(1, &vertexArrayObject);
	glDeleteBuffers(1, &indexBufferObject);
	glDeleteVertexArrays(1, &vertexBufferObject);
	glDeleteProgram(shaderProgram);
	delete(mesh);
}
