//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include <map>

#include <Setting/Setting.h>
#include <Memory/BxMemory.h>

#include "BxQueue.h"
#include "TextureMgr.h"
#include "../Buffer/VulkanVertexBuffer.h"
#include "../Render/VulkanRender.h"

namespace VulkanEngine
{
    class VulkanContext
    {
    public:
        VulkanContext(
            const Setting*                 pSetting,
            const Memory::MemoryPoolArena& arena);

        ~VulkanContext();

        void initialize();
        void run();

        INLINE const Setting* GetSetting() const { return m_pSetting; }
        INLINE GLFWwindow*    GetWindow()  const { return m_pWindow;  }

        INLINE Scene::RenderScene* GetRenderScene(
            const UINT sceneIndex) const
        {
            assert(sceneIndex < m_pRenderSceneList.size());

            return m_pRenderSceneList[sceneIndex];
        }

        INLINE void AddScene(
            Scene::RenderScene* pNewScene)
        {
            m_sceneNum++;

            assert(m_sceneNum <= m_pSetting->maxSceneNum);

            m_pRenderSceneList.push_back(pNewScene);
        }

        INLINE const BOOL IsSamplerAnisotropySupport()           const { return m_isSamplerAnisotropySupport; }
        INLINE const VkPhysicalDeviceProperties& GetHwProps()    const { return m_hwProps;                    }
        INLINE const VkPhysicalDeviceFeatures&   GetHwFeatures() const { return m_hwFeatures;                 }

    private:
        BOOL initWindow();
        BOOL initRenderScene();
        BOOL initVulkan();
        BOOL update(const float delta);
        BOOL draw();

        // Initialization sub-tasks
        BOOL createInstance();
        BOOL createSurface();
        BOOL initHwDevice();
        BOOL initDevice();
        BOOL createSwapchain();
        BOOL createRender();
        BOOL createSemaphores();

        /// Context attributes
        // Window pointer and settings
        const Setting*    m_pSetting;
        GLFWwindow*       m_pWindow;
        const std::string m_windowName;
        const std::string m_engineName;

        // Memory
        Memory::MemoryPoolArena m_arena;

        /// Core vulkan components
        VDeleter<VkInstance>          m_vkInstance;
        VDeleter<VkSurfaceKHR>        m_vkSurface;
        std::vector<VkPhysicalDevice> m_vkActiveHwGpuDeviceList;
        VDeleter<VkDevice>            m_vkDevice;

        // Properties
        VkPhysicalDeviceProperties m_hwProps;
        VkPhysicalDeviceFeatures   m_hwFeatures;

        // Timer
        float m_prevTime;
        float m_deltaTime;

        /// Managers
        // Queue
        Mgr::QueueMgr m_queueMgr;

        // Command buffer
        std::unique_ptr<Mgr::CmdBufferMgr>  m_pCmdBufferMgr;

        // Image
        std::unique_ptr<Mgr::TextureMgr>    m_pTextureMgr;

        // Descriptor
        std::unique_ptr<Mgr::DescriptorMgr> m_pDescriptorMgr;

        /// Hardware properties
        // Avaliable hardware devices are listed with descending order
        std::multimap<UINT, VkPhysicalDevice, std::greater<UINT>> m_avaliableHwGpuDevices;

        // Swapchain
        VDeleter<VkSwapchainKHR>               m_swapchain;
        Utility::SwapChainHwProperties         m_swapchainHwProperties;
        VkExtent2D                             m_swapchainExtent;
        VkSurfaceFormatKHR                     m_swapchainSurfaceFormat;
        std::vector<Texture::VulkanTexture2D*> m_pSwapchainTextures;

        // Render
        std::unique_ptr<Render::VulkanRenderBase> m_pRender;

        // Semaphores
        VDeleter<VkSemaphore> m_renderSemaphore;
        VDeleter<VkSemaphore> m_presentSemaphore;

        // Scene
        std::vector<Scene::RenderScene*> m_pRenderSceneList;
        UINT                             m_activeSceneIndex;
        UINT                             m_sceneNum;

        // Sampler anisotropy support
        BOOL m_isSamplerAnisotropySupport;

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