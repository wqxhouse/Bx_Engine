#pragma once

#include "../Core/VulkanPCH.h"
#include "../Core/VulkanUtility.h"
#include "Setting.h"

#include <map>

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
    BOOL initHwDevice();
    BOOL initDevice();

    // Core vulkan components
    VDeleter<VkInstance>          m_vkInstance;
    std::vector<VkPhysicalDevice> m_vkActiveHwGpuDeviceList;
    VDeleter<VkDevice>            m_vkDevice;

    /// Context attributes
    // Window pointer and settings
    Setting*          m_pSetting;
    GLFWwindow*       m_pWindow;
    const std::string m_windowName;
    const std::string m_engineName;

    // Timer
    float m_prevTime;
    float m_deltaTime;

    // Hardware properties
    // Avaliable hardware devices are listed with descending order
    std::multimap<UINT, VkPhysicalDevice, std::greater<UINT>> m_avaliableHwGpuDevices;
    QueueFamilyIndices                                        m_hwQueueIndices;

#if _DEBUG
    BOOL initDebug();
    VDeleter<VkDebugUtilsMessengerEXT> m_vkDebugMsg;
#endif

    // Extension
    UINT         m_extCount;
    const char** m_extensions;
};