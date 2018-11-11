#pragma once

#include "BxQueue.h"
#include "Setting.h"
#include "../Core/VulkanPCH.h"
#include "../Core/VulkanUtility.h"
#include "../Shader/VulkanGraphicsShader.h"

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
    BOOL createSurface();
    BOOL initHwDevice();
    BOOL initDevice();
    BOOL createSwapchain();
	BOOL createGraphicsPipeline();

    /// Context attributes
    // Window pointer and settings
    Setting*          m_pSetting;
    GLFWwindow*       m_pWindow;
    const std::string m_windowName;
    const std::string m_engineName;

    // Core vulkan components
    VDeleter<VkInstance>          m_vkInstance;
    VDeleter<VkSurfaceKHR>        m_vkSurface;
    std::vector<VkPhysicalDevice> m_vkActiveHwGpuDeviceList;
    VDeleter<VkDevice>            m_vkDevice;

    // Timer
    float m_prevTime;
    float m_deltaTime;

    // Hardware properties
    // Avaliable hardware devices are listed with descending order
    std::multimap<UINT, VkPhysicalDevice, std::greater<UINT>> m_avaliableHwGpuDevices;

    // Swapchain
    VDeleter<VkSwapchainKHR>           m_swapchain;
    SwapChainHwProperties              m_swapchainHwProperties;
    std::vector<VkImage>	           m_swapchainImages;
    std::vector<VDeleter<VkImageView>> m_swapchainImagesView;
    VkExtent2D                         m_swapchainExtent;
    VkSurfaceFormatKHR                 m_swapchainSurfaceFormat;

    // Queue
    QueueMgr m_queueMgr;

    // Shader
    VulkanGraphicsShader* m_pShader;

	// Pipeline
    VDeleter<VkPipelineLayout> m_graphicsPipelineLayout;
    VDeleter<VkRenderPass>     m_renderPass;
    VDeleter<VkPipeline>       m_graphicsPipeline;

#if _DEBUG
    const static BOOL m_enableValidationLayer = TRUE;

    BOOL initDebug();
    VDeleter<VkDebugUtilsMessengerEXT> m_vkDebugMsg;
#else
    const static bool m_enableValidationLayer = FALSE;
#endif

    // Extension
    UINT         m_instanceExtCount;
    const char** m_instanceExtensions;

    BOOL         m_deviceExtSupport;

    const static std::vector<const char*> m_validationLayers;
    const static std::vector<const char*> m_deviceExts;
};