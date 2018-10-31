#include "VulkanContext.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);

VulkanContext::VulkanContext(
    Setting * pSetting)
{
}

VulkanContext::~VulkanContext()
{
}

void VulkanContext::initialize()
{
    BOOL status = BX_SUCCESS;

    status = initWindow();
    if (status == BX_FAIL)
    {
        printf("Failed to initialize window!\n");
        assert(BX_FAIL);
    }
}

void VulkanContext::run()
{
    while (glfwWindowShouldClose(m_pWindow) != FALSE)
    {
        glfwSwapBuffers(m_pWindow);
        glfwPollEvents();
    }

    glfwTerminate();
}

BOOL VulkanContext::initWindow()
{
    BOOL status = BX_SUCCESS;

    status = glfwInit();

    if (status == BX_SUCCESS)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        GLFWmonitor* pMonitor = ((m_pSetting->fullScreen == TRUE) ? glfwGetPrimaryMonitor() : NULL);
        m_pWindow = glfwCreateWindow(m_pSetting->resolution.width,
                                     m_pSetting->resolution.height,
                                     "BxEngine Vulkan",
                                     pMonitor,
                                     NULL);

        if (m_pWindow != NULL)
        {
            glfwSetKeyCallback(m_pWindow, key_callback);
            glfwSetCursorPosCallback(m_pWindow, mouse_callback);
        }
        else
        {
            printf("Fail to create GLFW window!\n");
            status = BX_FAIL;
        }
    }
    else
    {
        printf("Fail to initialize GLFW!\n");
    }

    return status;
}

BOOL VulkanContext::initVulkan()
{
    BOOL status = BX_SUCCESS;

    createInstance();

    return status;
}

BOOL VulkanContext::createInstance()
{
    BOOL status = BX_SUCCESS;

    return status;
}

void key_callback(
    GLFWwindow* window,
    int         key,
    int         scancode,
    int         action,
    int         mode)
{
    // When a user presses the escape key, we set the WindowShouldClose property to true,
    // closing the application
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_ESCAPE)
        {
            glfwSetWindowShouldClose(window, TRUE);
        }
        else
        {
        }
    }
    else if (action == GLFW_RELEASE)
    {
    }
}

void mouse_callback(
    GLFWwindow* window,
    double      x_pos,
    double      y_pos)
{
}