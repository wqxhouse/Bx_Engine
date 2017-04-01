#include "OpenGLContext.h"
#include "../Model/Mesh/ObjModelLoader.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);

OpenGLContext::OpenGLContext(const Setting & setting)
	:callbackInfo()
{
	this->setting = setting;
	this->m_scene = new Scene(setting);
}

void OpenGLContext::initialize()
{
	//Initialize the GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(setting.width, setting.height, "OpenGL Template", nullptr, nullptr);
	if (window == nullptr)
	{
		printf("Fail to create GLFW window.\n");
		glfwTerminate();
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
	}
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	//glfwGetFramebufferSize(window, width, height);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glViewport(0, 0, setting.width, setting.height);

	glClearColor(0.0f, 0.0f, 0.6f, 1.0f);

#if _DEBUG
	printf("Success initialize OpenGL.\n");
#endif
	
	assert(m_scene->initialize() == 0);

#if _DEBUG
	printf("Success initialize render scene.\n");
#endif
}

void OpenGLContext::run()
{
	initialize();
	GLfloat prevTime = 0.0f;
	GLfloat deltaTime = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		GLfloat curTime = (GLfloat)glfwGetTime();
		deltaTime = curTime - prevTime;
		prevTime = curTime;

		glfwPollEvents();

		m_scene->update(deltaTime);

		//Start Rendering
		glClear(GL_COLOR_BUFFER_BIT);
		m_scene->draw();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

OpenGLContext::~OpenGLContext()
{
	delete(m_scene);
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
			//m_proj_camera.updateKeyboardMotion(key, 1);
			callbackInfo.keyboardCallBack[key] = 1;
		}
	}
	else if (action == GLFW_RELEASE)
	{
		//m_proj_camera.updateKeyboardMotion(key, 0);
		callbackInfo.keyboardCallBack[key] = 0;
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
	}
	else
	{
		callbackInfo.cursorPosCallBack.delta_x = x_pos - prevPosX;
		callbackInfo.cursorPosCallBack.delta_y = y_pos - prevPosY;
		//m_proj_camera.updateMouseMotion(x_pos - prevPosX, y_pos - prevPosY);
	}
	//m_context.m_proj_camera.updateMouseMotion(halfWidth - x_pos, halfHeight - y_pos);
}