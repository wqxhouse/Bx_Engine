#include "VulkanContext.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);

VulkanContext::VulkanContext(
    Setting* pSetting)
    : m_pSetting(pSetting),
      m_pWindow(NULL),
      m_windowName("BxEngine Vulkan"),
      m_engineName("BxEngine"),
      m_prevTime(0.0f),
      m_deltaTime(0.0f),
      m_extCount(0),
      m_extensions(NULL)
{
    // Set resource release callback functions
    m_vkInstance = { vkDestroyInstance };
}

VulkanContext::~VulkanContext()
{
}

void VulkanContext::initialize()
{
    BOOL status = BX_SUCCESS;

    status = initWindow();
    if (status == BX_SUCCESS)
    {
        status = initVulkan();

        if (status == BX_SUCCESS)
        {
            printf("Successfully initialize Vulkan window!\n");
        }
        else
        {
            printf("Failed to initialize Vulkan!\n");
            assert(BX_FAIL);
        }
    }
    else
    {
        printf("Failed to initialize window!\n");
        assert(BX_FAIL);
    }
}

void VulkanContext::run()
{
    while (glfwWindowShouldClose(m_pWindow) == FALSE)
    {
        glfwSwapBuffers(m_pWindow);
        glfwPollEvents();
    }

    glfwDestroyWindow(m_pWindow);
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
                                     m_windowName.data(),
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

    if (status == BX_SUCCESS)
    {
        status = createInstance();
        if (status == BX_FAIL)
        {
            printf("Failed to create Vulkan Instance!\n");
            assert(BX_FAIL);
        }
    }

    return status;
}

BOOL VulkanContext::createInstance()
{
    BOOL status = BX_SUCCESS;

    m_extensions = glfwGetRequiredInstanceExtensions(&m_extCount);

    VkApplicationInfo appInfo = {};
    appInfo.sType             = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName  = m_windowName.data();
    appInfo.apiVersion        = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName       = m_engineName.data();
    appInfo.engineVersion     = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion        = VK_VERSION_1_1;

    VkInstanceCreateInfo instanceInfo    = {};
    instanceInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo        = &appInfo;
    instanceInfo.enabledExtensionCount   = m_extCount;
    instanceInfo.ppEnabledExtensionNames = m_extensions;
    instanceInfo.enabledLayerCount       = 0; // TODO: Enable validation layers

    VkResult result = vkCreateInstance(&instanceInfo, NULL, m_vkInstance.replace());
    status = ((result == VK_SUCCESS) ? BX_SUCCESS : BX_FAIL);

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