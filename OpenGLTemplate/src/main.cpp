/*

	@author: Xiangshun Bei(bxs3514)
	@date: 07/20/2016

*/

#include <../../../OpenGLTemplate/header/PCH.h>

const int width = 1280;
const int height = 720;

GLFWwindow* window;

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
		glClearColor(0.0f, 0.0f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}