#include "Scene.h"
#include "../Model/Mesh/ObjModelLoader.h"

//Image loader
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Scene::Scene(const Setting & setting)
	:m_proj_camera(glm::vec3(-20, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), 5.0f, (float)setting.width / (float)setting.height)
{
	this->setting = setting;
}

int Scene::initialize()
{
	////Load model and texture(Hardcode here)
	//ObjModelLoader objLoader;
	//objLoader.LoadModel("../resources/models/farmhouse/farmhouse_tri.obj", &(pModel->mesh));
	Transform* pTrans = new Transform(glm::vec3(), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f));
	addModel("../resources/models/farmhouse/farmhouse_tri.obj", pTrans);


	//Create vertex buffer and vertex array

	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	//Set vertex buffer data
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	//glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sceneModelsPtr[0]->mesh->vertexBuffer.size() * sizeof(GLfloat), sceneModelsPtr[0]->mesh->vertexBuffer.data(), GL_STATIC_DRAW);

	//Create index buffer data and bind to VAO
	glGenBuffers(1, &indexBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sceneModelsPtr[0]->mesh->indexBuffer.size() * sizeof(GLuint), sceneModelsPtr[0]->mesh->indexBuffer.data(), GL_STATIC_DRAW);

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

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (GLvoid*)(offsetof(Mesh::Vertex, Mesh::Vertex::texCoords)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//Create texture and set sampler
	glGenTextures(1, &textureBinder);
	glBindTexture(GL_TEXTURE_2D, textureBinder);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	int texWidth, texHeight, texChannels;
	stbi_uc* imgData = stbi_load("../resources/textures/cube/wall.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(imgData);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	//Compile shaders
	ShaderCompiler m_shaderCompiler;
	const char* vertexShaderFile = "SimpleTexture.vert";
	const char* fragmentShaderFile = "SimpleTexture.frag";
	int hs = m_shaderCompiler.compileShader(vertexShaderFile, fragmentShaderFile, &shaderProgram);
	//int hs = m_shaderCompiler.compileShader("../resources/shaders/", vertexShaderFile, "../resources/shaders/", fragmentShaderFile, &shaderProgram);

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
	//transform = glm::translate(transform, glm::vec3(10.0f, 10.0f, 10.0f));
	//transform = glm::rotate(transform, glm::radians(180.0f) * timeValue, glm::vec3(0, 1, 0));
	transform = glm::scale(transform, glm::vec3(.5f, .5f, .5f));

	GLint glWorldMatrixLocation = glGetUniformLocation(shaderProgram, "world");
	glUniformMatrix4fv(glWorldMatrixLocation, 1, GL_FALSE, glm::value_ptr(transform));

	GLint glViewMatrixLocation = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(glViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(m_proj_camera.getViewMatrix()));

	GLint glProjectionLocation = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(glProjectionLocation, 1, GL_FALSE, glm::value_ptr(m_proj_camera.getProjectionMatrix()));
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureBinder);
	glUniform1i(glGetUniformLocation(shaderProgram, "sampler"), 0);

	glBindVertexArray(vertexArrayObject);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//Wireframe mode
	glDrawElements(GL_TRIANGLES, sceneModelsPtr[0]->mesh->indexBuffer.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

Scene::~Scene()
{
	glDeleteBuffers(1, &vertexArrayObject);
	glDeleteBuffers(1, &indexBufferObject);
	glDeleteTextures(1, &textureBinder);
	glDeleteVertexArrays(1, &vertexBufferObject);
	glDeleteProgram(shaderProgram);

	for (Model* model : sceneModelsPtr)
	{
		delete(model);
	}
	sceneModelsPtr.clear();
	//delete(pModel);
}

void Scene::addModel(const std::string & modelFile, Transform * modelTrans)
{
	Model* pModel = new Model(modelFile, modelTrans);
	sceneModelsPtr.push_back(pModel);
}
