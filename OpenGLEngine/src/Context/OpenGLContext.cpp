#include "OpenGLContext.h"
#include "../Model/Mesh/ObjModelLoader.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);

OpenGLContext::OpenGLContext(const Setting & setting)
	:callbackInfo(), m_scene(setting)
{
	this->setting = setting;
}

void OpenGLContext::initialize()
{
	//Initialize the GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	glfwWindowHint(GLFW_SAMPLES, setting.m_graphicsSetting.antialasing);// 4x MSAA

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

	glEnable(GL_MULTISAMPLE);//Enable anti-alasing

	glEnable(GL_CULL_FACE);//Enable face culling
	glCullFace(GL_BACK);// Back face culling
	glFrontFace(GL_CCW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	glEnable(GL_STENCIL_TEST);
	//glStencilFunc(GL_EQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilMask(0xFF);

	glfwGetFramebufferSize(window, (int*)(&fbWidth), (int*)(&fbHeight));
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glViewport(0, 0, setting.width, setting.height);

	glClearColor(0.0f, 0.0f, 0.6f, 1.0f);

#if _DEBUG
	printf("Success initialize OpenGL.\n");
#endif
	
	assert(m_scene.initialize() == 0);

#if _DEBUG
	printf("Success initialize render scene.\n");
#endif
}

void OpenGLContext::run()
{
	initialize();
	GLfloat prevTime = 0.0f;
	GLfloat deltaTime = 0.0f;

    GLboolean param;
    glGetBooleanv(GL_CLIP_ORIGIN, &param);
    glClipControl(GL_UPPER_LEFT, GL_NEGATIVE_ONE_TO_ONE);
    glGetBooleanv(GL_CLIP_ORIGIN, &param);

	while (!glfwWindowShouldClose(window))
	{
		GLfloat curTime = (GLfloat)glfwGetTime();
		deltaTime = curTime - prevTime;
		prevTime = curTime;

		glfwPollEvents();

		m_scene.update(deltaTime);

		//Start Rendering
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		m_scene.draw();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

OpenGLContext::~OpenGLContext()
{
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When a user presses the escape key, we set the WindowShouldClose property to true, closing the application
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

		prevPosX = x_pos;
		prevPosY = y_pos;
		//m_proj_camera.updateMouseMotion(x_pos - prevPosX, y_pos - prevPosY);
	}
	//printf("Callback: %lf, %lf, %lf, %lf\n", prevPosX, x_pos, prevPosY, y_pos);
	//m_context.m_proj_camera.updateMouseMotion(halfWidth - x_pos, halfHeight - y_pos);
}