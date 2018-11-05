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
      m_extensions(NULL),
      m_graphicsQueuePriority(1.0f),
      m_computeQueuePriority(1.0f)
{
    // Set resource release callback functions
    m_vkInstance = { vkDestroyInstance };
    m_vkDevice   = { vkDestroyDevice   };

#if _DEBUG
    m_vkDebugMsg = { m_vkInstance, VulkanUtility::DestroyDebugUtilsMessenger };
#endif
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

#if _DEBUG
    if (status == BX_SUCCESS)
    {
        status = initDebug();
        if (status == BX_FAIL)
        {
            printf("Failed to initialize Validation layers!\n");
            assert(BX_FAIL);
        }
    }
#endif
    if (status == BX_SUCCESS)
    {
        status = initHwDevice();
        if (status == BX_FAIL)
        {
            printf("Failed to get hardware devices!\n");
            assert(BX_FAIL);
        }
    }

    if (status == BX_SUCCESS)
    {
        status = initDevice();
        if (status == BX_FAIL)
        {
            printf("Failed to initialize Vulkan Device!\n");
            assert(BX_FAIL);
        }
    }

    return status;
}

BOOL VulkanContext::createInstance()
{
    BOOL status = BX_SUCCESS;

    UINT enabledLayerCount                 = 0;
    const char* const* ppEnabledLayerNames = NULL;

    // Checking validation layer support
    if ((m_enableValidationLayer                                                              == TRUE) &&
        (VulkanUtility::CheckValidationLayerSupport(&enabledLayerCount, &ppEnabledLayerNames) == FALSE))
    {
        status = FALSE;
    }

    if (status == TRUE)
    {
#if _DEBUG
        std::vector<const char*> extensions = VulkanUtility::GetRequiredExts();
        m_extCount   = static_cast<UINT>(extensions.size());
        m_extensions = extensions.data();
#else
        m_extensions = glfwGetRequiredInstanceExtensions(&m_extCount);
#endif

        VkApplicationInfo appInfo = {};
        appInfo.sType             = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName  = m_windowName.data();
        appInfo.apiVersion        = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName       = m_engineName.data();
        appInfo.engineVersion     = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion        = VK_API_VERSION_1_1;

        VkInstanceCreateInfo instanceInfo    = {};
        instanceInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo        = &appInfo;
        instanceInfo.enabledExtensionCount   = m_extCount;
        instanceInfo.ppEnabledExtensionNames = m_extensions;
        instanceInfo.enabledLayerCount       = enabledLayerCount;
        instanceInfo.ppEnabledLayerNames     = ppEnabledLayerNames;

        VkResult result = vkCreateInstance(&instanceInfo, NULL, m_vkInstance.replace());
        status = ((result == VK_SUCCESS) ? BX_SUCCESS : BX_FAIL);
    }

    return status;
}

BOOL VulkanContext::initHwDevice()
{
    BOOL result = BX_SUCCESS;

    UINT gpuNum = 0;
    VkResult vkResult = vkEnumeratePhysicalDevices(m_vkInstance, &gpuNum, NULL);

    std::vector<VkPhysicalDevice> hwDevices(gpuNum);
    if (gpuNum > 0)
    {
        vkResult = vkEnumeratePhysicalDevices(m_vkInstance, &gpuNum, hwDevices.data());

        result = ((vkResult == VK_SUCCESS) ? BX_SUCCESS : BX_FAIL);
    }
    else
    {
        result = BX_FAIL;
    }

    if (result == BX_SUCCESS)
    {
        for (const VkPhysicalDevice& hwDevice : hwDevices)
        {
            UINT score = VulkanUtility::GetHwDeviceScore(hwDevice);

            if (score > 0)
            {
                m_avaliableHwGpuDevices.insert(std::pair<UINT, VkPhysicalDevice>(score, hwDevice));
            }
        }

        // Pick up the GPU with top score
        // TODO: Heriustic choose GPUs
        // TODO: Parallel using avaliable GPUs
        m_vkActiveHwGpuDeviceList.push_back(m_avaliableHwGpuDevices.begin()->second);

        m_hwQueueIndices = VulkanUtility::GetQueueFamilies(m_vkActiveHwGpuDeviceList[0]);

        if (m_hwQueueIndices.isValid() == FALSE)
        {
            result = BX_FAIL;
        }
    }

    return result;
}

BOOL VulkanContext::initDevice()
{
    BOOL result = BX_SUCCESS;

    VkDeviceQueueCreateInfo queueCreateInfo[2] = { {}, {} };
    // Graphics queues
    queueCreateInfo[0].sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo[0].queueFamilyIndex = m_hwQueueIndices.graphicsFamilyIndex;
    queueCreateInfo[0].queueCount       = 1;
    queueCreateInfo[0].pQueuePriorities = &m_graphicsQueuePriority;
    // Compute queues
    queueCreateInfo[1].sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo[1].queueFamilyIndex = m_hwQueueIndices.computeFamilyIndex;
    queueCreateInfo[1].queueCount       = 1;
    queueCreateInfo[1].pQueuePriorities = &m_computeQueuePriority;

    VkPhysicalDeviceFeatures hwDeviceFeatures = {};

    VkDeviceCreateInfo deviceCreateInfo    = {};
    deviceCreateInfo.sType                 = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos     = queueCreateInfo;
    deviceCreateInfo.queueCreateInfoCount  = 2;
    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.pEnabledFeatures      = &hwDeviceFeatures;

    if (m_enableValidationLayer == TRUE)
    {
        deviceCreateInfo.enabledLayerCount   = static_cast<UINT>(VulkanUtility::m_validationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = VulkanUtility::m_validationLayers.data();
    }
    else
    {
        deviceCreateInfo.enabledLayerCount = 0;
    }

    VkResult vkResult = vkCreateDevice(m_vkActiveHwGpuDeviceList[0], &deviceCreateInfo, NULL, m_vkDevice.replace());
    result            = ((vkResult == VK_SUCCESS) ? BX_SUCCESS : BX_FAIL);

    vkGetDeviceQueue(m_vkDevice, m_hwQueueIndices.graphicsFamilyIndex, 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_vkDevice, m_hwQueueIndices.computeFamilyIndex,  0, &m_computeQueue);

    return result;
}

#if _DEBUG
BOOL VulkanContext::initDebug()
{
    BOOL result = BX_SUCCESS;

    VkDebugUtilsMessengerCreateInfoEXT debugMsgCreateInfo = {};
    debugMsgCreateInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugMsgCreateInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT   |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
    debugMsgCreateInfo.messageType     =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugMsgCreateInfo.pfnUserCallback = VulkanUtility::debugCallback;
    debugMsgCreateInfo.pUserData       = NULL;

    VulkanUtility::CreateDebugUtilsMessenger(m_vkInstance, &debugMsgCreateInfo, NULL, m_vkDebugMsg.replace());

    return result;
}
#endif

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