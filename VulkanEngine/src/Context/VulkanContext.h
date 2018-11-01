#pragma once

#include "../Core/VulkanPCH.h"
#include "../Core/VulkanUtility.h"
#include "Setting.h"

class VulkanContext
{
public:
    VulkanContext(Setting* pSetting);
    ~VulkanContext();

    void initialize();
    void run();

    inline Setting*    GetSetting() const { return m_pSetting; }
    inline GLFWwindow* GetWindow()  const { return m_pWindow;  }

private:
    BOOL initWindow();
    BOOL initVulkan();

    BOOL createInstance();
    BOOL initDebug();

    // Window pointer and settings
    Setting*          m_pSetting;
    GLFWwindow*       m_pWindow;
    const std::string m_windowName;
    const std::string m_engineName;

    // Timer
    float m_prevTime;
    float m_deltaTime;

    // Core vulkan components
    VDeleter<VkInstance> m_vkInstance;

#if _DEBUG
    VDeleter<VkDebugUtilsMessengerEXT> m_vkDebugMsg;
#endif

    // Extension
    UINT         m_extCount;
    const char** m_extensions;
};