#pragma once

#include "Scene.h"

class OpenGLContext
{
public:
	OpenGLContext(const Setting& setting);
	void run();
	~OpenGLContext();
private:
	void initialize();
#if _DEBUG
	GLint success;
	GLchar compileLog[512];
#endif
	GLFWwindow* window;
	Setting setting;

	//TODO: Multiple Scenes
	Scene *m_scene;

	GLfloat prevTime = 0.0f;
	GLfloat deltaTime = 0.0f;

	CallbackInfo callbackInfo;
};