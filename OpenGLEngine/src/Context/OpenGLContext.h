//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

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
    inline Scene*      GetScene()        { return m_pScene; }

private:
#if _DEBUG
	GLint success;
	GLchar compileLog[512];
#endif
	GLFWwindow* window;
	Setting* m_pSetting;

	//TODO: Multiple Scenes
	Scene* m_pScene;

	GLfloat prevTime = 0.0f;
	GLfloat deltaTime = 0.0f;

	unsigned int fbWidth = 0u;
	unsigned int fbHeight = 0u;
};