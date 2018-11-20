//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include <Setting/Setting.h>
#include <Model/Model.h>

#include "BxQueue.h"
#include "CmdBufferMgr.h"
#include "../Shader/VulkanGraphicsShader.h"
#include "../Buffer/VulkanVertexBuffer.h"

#include <map>

namespace VulkanEngine
{
    class VulkanContext
    {
    public:
        VulkanContext(const Setting* pSetting);
        ~VulkanContext();

        void initialize();
        void run();

        inline const Setting* GetSetting() const { return m_pSetting; }
        inline GLFWwindow*    GetWindow()  const { return m_pWindow;  }

    private:
        BOOL initWindow();
        BOOL initVulkan();
        BOOL draw();

        // Initialization sub-tasks
        BOOL createInstance();
        BOOL createSurface();
        BOOL initHwDevice();
        BOOL initDevice();
        BOOL createSwapchain();
        BOOL createGraphicsPipeline();
        BOOL createSwapchainFramebuffer();
        BOOL createSemaphores();

        /// Context attributes
        // Window pointer and settings
        const Setting*    m_pSetting;
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
        VDeleter<VkSwapchainKHR>             m_swapchain;
        Utility::SwapChainHwProperties       m_swapchainHwProperties;
        VkExtent2D                           m_swapchainExtent;
        VkSurfaceFormatKHR                   m_swapchainSurfaceFormat;
        std::vector<VkImage>	             m_swapchainImages;
        std::vector<VDeleter<VkImageView>>   m_swapchainImagesViews;
        std::vector<VDeleter<VkFramebuffer>> m_swapchainFramebuffers;

        // Queue
        Queue::QueueMgr m_queueMgr;

        // Shader
        std::unique_ptr<Shader::VulkanGraphicsShader> m_pShader;

        // Pipeline
        VDeleter<VkPipelineLayout> m_graphicsPipelineLayout;
        VDeleter<VkRenderPass>     m_renderPass;
        VDeleter<VkPipeline>       m_graphicsPipeline;

        // Command buffer
        std::unique_ptr<Mgr::CmdBufferMgr> m_pCmdBufferMgr;

        // Semaphores
        VDeleter<VkSemaphore> m_renderSemaphore;
        VDeleter<VkSemaphore> m_presentSemaphore;

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
}