/*

	@author: Xiangshun Bei(bxs3514)
	@date: 07/20/2016

*/

#include "Core/OpenGLPCH.h"
#include "../header/FirstTriangle.h"

const int width = 1280;
const int height = 720;
const int halfWidth = (width >> 1);
const int halfHeight = (height >> 1);

GLFWwindow* window;

FirstTriangle mFirstTriangle;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);

int initialize()
{
	//Initialize the GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(width, height, "OpenGL Template", nullptr, nullptr);
	if (window == nullptr)
	{
		printf("Fail to create GLFW window.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	//Initialize the GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		printf("Fail to initialize GLEW.\n");
		glfwTerminate();
		return -1;
	}
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	//glfwGetFramebufferSize(window, width, height);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glViewport(0, 0, width, height);

	glClearColor(0.0f, 0.0f, 0.6f, 1.0f);

	if (mFirstTriangle.Initialize() == -1)
	{
		return -1;
	}

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When a user presses the escape key, we set the WindowShouldClose property to true, 
	// closing the application
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		else
		{
			mFirstTriangle.m_proj_camera.updateKeyboardMotion(key, 1);
		}
	}
	else if(action == GLFW_RELEASE)
	{
		mFirstTriangle.m_proj_camera.updateKeyboardMotion(key, 0);
	}
}

void mouse_callback(GLFWwindow * window, double x_pos, double y_pos)
{
	static bool firstMouseCall = false;
	static double prevPosX = 0.0;
	static double prevPosY = 0.0;

	if (!firstMouseCall)
	{
		firstMouseCall = true;
		prevPosX = x_pos;
		prevPosY = y_pos;
		mFirstTriangle.m_proj_camera.updateMouseMotion(0.0, 0.0);
	}
	else
	{
		mFirstTriangle.m_proj_camera.updateMouseMotion(x_pos - prevPosX, y_pos - prevPosY);
	}
	//mFirstTriangle.m_proj_camera.updateMouseMotion(halfWidth - x_pos, halfHeight - y_pos);
}

int main()
{
	if (initialize() != 0)
	{
		printf("Fail to initialize OpenGL.\n");
		system("pause");
		return -1;
	}

	GLfloat prevTime = 0.0f;
	GLfloat deltaTime = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		GLfloat curTime = (GLfloat)glfwGetTime();
		deltaTime = curTime - prevTime;
		prevTime = curTime;

		glfwPollEvents();

		mFirstTriangle.update(deltaTime);

		//Start Rendering
		glClear(GL_COLOR_BUFFER_BIT);
		mFirstTriangle.draw();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}