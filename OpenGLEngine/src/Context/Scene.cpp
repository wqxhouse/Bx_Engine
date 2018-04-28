#include "../Core/Utility.h"
#include "Scene.h"
#include "../Model/Mesh/ObjModelLoader.h"

//Image loader
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Scene::Scene(const Setting & setting)
	: m_directionalLight(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f)),
	  m_activeCamera(0)
{
	this->setting = setting;
}

int Scene::initialize()
{
	m_pCameraList.push_back(
		new ProspectiveCamera(glm::vec3(5.0f, 2.0f, 2.0f), glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0), 5.0f, (float)setting.width / (float)setting.height));

	m_pCameraList.push_back(
		new ProspectiveCamera(glm::vec3(-5.0f, 2.0f, 2.0f), glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0), 5.0f, (float)setting.width / (float)setting.height));

	////Load model and texture(Hardcode here)
	//ObjModelLoader objLoader;
	//objLoader.LoadModel("../resources/models/farmhouse/farmhouse_tri.obj", &(pModel->mesh));
	Transform* pTrans = new Transform(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f));
	addModel("../../resources/models/cornellbox/CornellBox-Sphere.obj", pTrans);
	//addModel("../resources/models/sphere/sphere.obj", pTrans);

	Transform* pTrans2 = new Transform(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f));
	addModel("../../resources/models/sphere/sphere.obj", pTrans2);

	//Create vertex buffer and vertex array

	glGenVertexArrays(2, vertexArrayObject);
	glGenBuffers(2, vertexBufferObject);
	glGenBuffers(2, indexBufferObject);
	glGenBuffers(1, &lightParagBuffer);
	glGenBuffers(1, &transformHandle);
	glGenTextures(2, textureHandle);

	glBindVertexArray(vertexArrayObject[0]);

	//Set vertex buffer data
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject[0]);
	//glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sceneModelsPtr[0]->mesh->vertexBuffer.size() * sizeof(GLfloat),
		         sceneModelsPtr[0]->mesh->vertexBuffer.data(), GL_STATIC_DRAW);

	//Create index buffer data and bind to VAO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject[0]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sceneModelsPtr[0]->mesh->indexBuffer.size() * sizeof(GLuint),
		         sceneModelsPtr[0]->mesh->indexBuffer.data(), GL_STATIC_DRAW);

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), 
		                 (GLvoid*)(offsetof(Mesh::Vertex, Mesh::Vertex::position)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), 
		                 (GLvoid*)(offsetof(Mesh::Vertex, Mesh::Vertex::normal)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), 
		                 (GLvoid*)(offsetof(Mesh::Vertex, Mesh::Vertex::texCoords)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(vertexArrayObject[1]);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject[1]);
	glBufferData(GL_ARRAY_BUFFER, sceneModelsPtr[1]->mesh->vertexBuffer.size() * sizeof(GLfloat),
		         sceneModelsPtr[1]->mesh->vertexBuffer.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sceneModelsPtr[1]->mesh->indexBuffer.size() * sizeof(GLuint),
		         sceneModelsPtr[1]->mesh->indexBuffer.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex),
		                 (GLvoid*)(offsetof(Mesh::Vertex, Mesh::Vertex::position)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex),
		                 (GLvoid*)(offsetof(Mesh::Vertex, Mesh::Vertex::normal)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex),
		                 (GLvoid*)(offsetof(Mesh::Vertex, Mesh::Vertex::texCoords)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	//Create texture and set sampler
	glBindTexture(GL_TEXTURE_2D, textureHandle[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	int texWidth, texHeight, texChannels;
	stbi_uc* imgData = stbi_load("../resources/textures/teaport/wall.jpg", 
		                         &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(imgData);

	glBindTexture(GL_TEXTURE_2D, textureHandle[1]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	imgData = stbi_load("../resources/textures/sphere/wall.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	
	//Compile shaders
	ShaderCompiler m_shaderCompiler;
	const char* vertexShaderFile = "SimpleTexture.vert";
	const char* fragmentShaderFile = "SimpleTexture.frag";
	int hs = m_shaderCompiler.compileShader(vertexShaderFile, fragmentShaderFile, &shaderProgram);

	if (hs != 0)
	{
		printf("Fail to compile shaders.\n");
		return -1;
	}

	/*transformHandle = glGetUniformBlockIndex(shaderProgram, "trans");
	glGetActiveUniformBlockiv(shaderProgram, transformHandle, GL_UNIFORM_BLOCK_DATA_SIZE, &transformBufferSize);
	transformBufferData = (GLubyte*)malloc(transformBufferSize);

	glBindBuffer(GL_UNIFORM_BUFFER, transformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, transformBufferSize, transformBufferData, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, transformHandle, transformBuffer);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);*/

	lightParagHandle = glGetUniformBlockIndex(shaderProgram, "light");
	glGetActiveUniformBlockiv(shaderProgram, lightParagHandle, GL_UNIFORM_BLOCK_DATA_SIZE, &lightStructSize);
	lightDataBuffer = (GLubyte*)malloc(lightStructSize);

	GLuint indices[2];
	glGetUniformIndices(shaderProgram, 2, lightMembers, indices);

	GLint offsets[2];
	glGetActiveUniformsiv(shaderProgram, 2, indices, GL_UNIFORM_OFFSET, offsets);

	memcpy(lightDataBuffer + offsets[0], &(m_directionalLight.getDir()), sizeof(glm::vec3));
	memcpy(lightDataBuffer + offsets[1], &(m_directionalLight.getLightColor()), sizeof(glm::vec3));

	glBindBuffer(GL_UNIFORM_BUFFER, lightParagBuffer);
	glBufferData(GL_UNIFORM_BUFFER, lightStructSize, lightDataBuffer, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, lightParagHandle, lightParagBuffer);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	return 0;
}

void Scene::update(float deltaTime)
{
	UINT cameraCount = m_pCameraList.size();

	assert(cameraCount > 0);

	if (callbackInfo.keyboardCallBack[GLFW_KEY_1] == 1)
	{
		m_activeCamera = 0;
	}
	else if (callbackInfo.keyboardCallBack[GLFW_KEY_2] == 1)
	{
		m_activeCamera = (1 < cameraCount) ? 1 : m_activeCamera;
	}
	else if (callbackInfo.keyboardCallBack[GLFW_KEY_3] == 1)
	{
		m_activeCamera = (2 < cameraCount) ? 2 : m_activeCamera;
	}

	m_pCameraList[m_activeCamera]->update(deltaTime);
}

void Scene::draw()
{
	glUseProgram(shaderProgram);

	GLfloat timeValue = (GLfloat)glfwGetTime();
	GLfloat colorValue = 1.0f;// (GLfloat)(sin(timeValue) / 2) + 0.5;
	GLint glVertexColorLocation = glGetUniformLocation(shaderProgram, "uniformColor");
	glUniform3f(glVertexColorLocation, colorValue, colorValue, colorValue);

	ProspectiveCamera* activeCamPtr = static_cast<ProspectiveCamera*>(m_pCameraList[m_activeCamera]);

	for (size_t i = 0; i < 1; ++i)
	{
		glm::mat4 rotation;
		//rotation = glm::rotate(rotation, glm::radians(180.0f) * timeValue, glm::vec3(0, 1, 0));

		GLint glRotationMatHandle = glGetUniformLocation(shaderProgram, "rot");
		glUniformMatrix4fv(glRotationMatHandle, 1, GL_FALSE, glm::value_ptr(rotation));

		glm::mat4 transform;
		transform = glm::translate(transform, sceneModelsPtr[i]->trans->pos/*glm::vec3(10.0f, 10.0f, 10.0f)*/);
		//transform *= rotation; //glm::rotate(transform, glm::radians(180.0f) * timeValue, glm::vec3(0, 1, 0));
		//transform = glm::scale(transform, glm::vec3(.5f, .5f, .5f));

		GLint glWorldMatrixLocation = glGetUniformLocation(shaderProgram, "world");
		glUniformMatrix4fv(glWorldMatrixLocation, 1, GL_FALSE, glm::value_ptr(transform));

		//glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0, 1, 0));
		GLint glViewMatrixLocation = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(glViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(activeCamPtr->getViewMatrix()/*view*/));

		GLint glProjectionLocation = glGetUniformLocation(shaderProgram, "proj");
		glUniformMatrix4fv(glProjectionLocation, 1, GL_FALSE, glm::value_ptr(activeCamPtr->getProjectionMatrix()));

		GLint glEyeHandle = glGetUniformLocation(shaderProgram, "eye");
		glUniformMatrix3fv(glEyeHandle, 1, GL_FALSE, glm::value_ptr(activeCamPtr->getTrans().front));

		glActiveTexture(GL_TEXTURE0);

		glBindTexture(GL_TEXTURE_2D, textureHandle[i]);
		glUniform1i(glGetUniformLocation(shaderProgram, "sampler"), 0);

		glBindVertexArray(vertexArrayObject[i]);

		if (setting.polyMode == setting.PolyMode::WIREFRAME)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//Wireframe mode
			glDrawElements(GL_TRIANGLES, sceneModelsPtr[i]->mesh->indexBuffer.size(), GL_UNSIGNED_INT, 0);
		}
		else if (setting.polyMode == setting.PolyMode::TRIANGLE)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);
			glDrawElements(GL_TRIANGLES, sceneModelsPtr[i]->mesh->indexBuffer.size(), GL_UNSIGNED_INT, 0);
		}
	}
	glBindVertexArray(0);
}

Scene::~Scene()
{
	glDeleteBuffers(2, vertexArrayObject);
	glDeleteBuffers(2, indexBufferObject);
	glDeleteBuffers(1, &transformBuffer);
	glDeleteBuffers(1, &normalTransformBuffer);
	glDeleteBuffers(1, &lightParagBuffer);
	glDeleteTextures(2, textureHandle);
	glDeleteVertexArrays(2, vertexBufferObject);
	glDeleteProgram(shaderProgram);

	SafeFree(lightDataBuffer);
	/*SafeFree(transformBufferData);
	SafeFree(normalTransformBufferData);*/

	for (Model* model : sceneModelsPtr)
	{
		delete(model);
	}
	sceneModelsPtr.clear();

	for (Camera* pCamera : m_pCameraList)
	{
		switch (pCamera->m_type)
		{
		case Camera::CameraType::PROJECT_CAM:
			delete static_cast<ProspectiveCamera*>(pCamera);
			break;
		case Camera::CameraType::ORTHO_CAM:
			delete static_cast<OrthographicCamera*>(pCamera);
			break;
		default:
			break;
		}
	}
	//delete(pModel);
}

void Scene::addModel(const std::string & modelFile, Transform * modelTrans)
{
	Model* pModel = new Model(modelFile, modelTrans);
	sceneModelsPtr.push_back(pModel);
}
