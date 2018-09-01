#pragma once

#include "Scene.h"

class OpenGLContext
{
public:
	OpenGLContext(Setting* pSetting);
	~OpenGLContext();

    void initialize();
    void run();

    inline GLFWwindow* GetWindow() const { return window;   }
    inline Scene*      GetScene()        { return &m_scene; }

private:
#if _DEBUG
	GLint success;
	GLchar compileLog[512];
#endif
	GLFWwindow* window;
	Setting setting;

	//TODO: Multiple Scenes
	Scene m_scene;

	GLfloat prevTime = 0.0f;
	GLfloat deltaTime = 0.0f;

	unsigned int fbWidth = 0u;
	unsigned int fbHeight = 0u;
};