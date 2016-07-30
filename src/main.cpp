/*

	@author: Xiangshun Bei(bxs3514)
	@date: 07/20/2016

*/

#include "../header/OpenGLPCH.h"
#include "../header/FirstTriangle.h"

const int width = 800;
const int height = 600;

GLFWwindow* window;

FirstTriangle mFirstTriangle;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

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

	//Initialize the GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		printf("Fail to initialize GLEW.\n");
		glfwTerminate();
		return -1;
	}

	//glfwGetFramebufferSize(window, width, height);

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
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

int main()
{
	if (initialize() != 0)
	{
		printf("Fail to initialize OpenGL.\n");
		return -1;
	}

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		//Start Rendering
		glClear(GL_COLOR_BUFFER_BIT);

		mFirstTriangle.Draw();

		glfwSwapBuffers(window);
	}

	mFirstTriangle.~FirstTriangle();
	glfwTerminate();
	return 0;
}