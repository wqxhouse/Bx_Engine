#include "OpenGLContext.h"
#include "../Model/Mesh/ObjModelLoader.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);

OpenGLContext::OpenGLContext(Setting* pSetting)
{
    this->m_pSetting = pSetting;
}

void OpenGLContext::initialize()
{
    //Initialize the GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    glfwWindowHint(GLFW_SAMPLES, m_pSetting->m_graphicsSetting.antialasing); // 4x MSAA

    window = glfwCreateWindow(m_pSetting->resolution.width,
                              m_pSetting->resolution.height,
                              "BXS OpenGL Engine",
                              // glfwGetPrimaryMonitor(), // Full Screen
                              NULL,
                              NULL);
    if (window == NULL)
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
    glCullFace(GL_BACK);   //Back face culling
    glFrontFace(GL_CCW);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);

    glEnable(GL_STENCIL_TEST);
    //glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilMask(0xFF);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glfwGetFramebufferSize(window,
                           reinterpret_cast<int*>(&fbWidth),
                           reinterpret_cast<int*>(&fbHeight));

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glViewport(0, 0, m_pSetting->resolution.width, m_pSetting->resolution.height);

#if _DEBUG
    printf("Success initialize OpenGL.\n");
#endif

    m_pScene = new Scene(m_pSetting);
    assert(m_pScene->initialize() == TRUE);

#if _DEBUG
    printf("Success initialize render scene.\n");
#endif
}

void OpenGLContext::run()
{
    GLfloat prevTime = 0.0f;
    GLfloat deltaTime = 0.0f;
    
    while (!glfwWindowShouldClose(window))
    {
        GLfloat curTime = (GLfloat)glfwGetTime();
        deltaTime = curTime - prevTime;
        prevTime = curTime;

        //printf("Second per frame: %f\n FPS: %f\n", deltaTime, 1.0f / deltaTime);
        m_pScene->update(deltaTime);

        //Start Rendering
        m_pScene->preDraw();
        m_pScene->draw();
        m_pScene->postDraw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

OpenGLContext::~OpenGLContext()
{
    SafeDelete(m_pScene);
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
            callbackInfo.keyboardCallBack[key] = 1;
        }
    }
    else if (action == GLFW_RELEASE)
    {
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
    }
}