//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "VulkanContext.h"

#include <set>

using namespace VulkanEngine::Utility;
using namespace VulkanEngine::Queue;

namespace VulkanEngine
{
    const std::vector<const char*> VulkanContext::m_validationLayers =
    {
        "VK_LAYER_LUNARG_standard_validation"
    };

    const std::vector<const char*> VulkanContext::m_deviceExts =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
    void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);

    VulkanContext::VulkanContext(
        const Setting*                 pSetting,
        const Memory::MemoryPoolArena& arena)
        : m_pSetting(pSetting),
          m_pWindow(NULL),
          m_windowName("BxEngine Vulkan"),
          m_engineName("BxEngine"),
          m_arena(arena),
          m_sceneNum(0),
          m_activeSceneIndex(0),
          m_prevTime(0.0f),
          m_deltaTime(0.0f),
          m_instanceExtCount(0),
          m_instanceExtensions(NULL),
          m_deviceExtSupport(FALSE)
    {
        // Set resource release callback functions
        m_vkInstance       = { vkDestroyInstance                   };
        m_vkSurface        = { m_vkInstance, vkDestroySurfaceKHR   };
        m_vkDevice         = { vkDestroyDevice                     };
        m_swapchain        = { m_vkDevice, vkDestroySwapchainKHR   };
        m_renderSemaphore  = { m_vkDevice, vkDestroySemaphore      };
        m_presentSemaphore = { m_vkDevice, vkDestroySemaphore      };

#if _DEBUG
        m_vkDebugMsg       = { m_vkInstance, VulkanUtility::DestroyDebugUtilsMessenger };
#endif
    }

    VulkanContext::~VulkanContext()
    {}

    void VulkanContext::initialize()
    {
        BOOL status = BX_SUCCESS;

        status = initWindow();

        assert(status == BX_SUCCESS);

        if (status == BX_SUCCESS)
        {
            status = initRenderScene();
        }

        assert(status == BX_SUCCESS);

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
        BOOL status = BX_SUCCESS;

        float prevTime  = 0.0f;
        float deltaTime = 0.0f;

        while (glfwWindowShouldClose(m_pWindow) == FALSE)
        {
            float curTime = static_cast<float>(glfwGetTime());
            deltaTime     = curTime - prevTime;
            prevTime      = curTime;

            glfwSwapBuffers(m_pWindow);
            glfwPollEvents();

            status = update(deltaTime);

            assert(status == BX_SUCCESS);

            status = draw();

            assert(status == BX_SUCCESS);
        }

        vkDeviceWaitIdle(m_vkDevice);

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
                glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

    BOOL VulkanContext::initRenderScene()
    {
        BOOL status = BX_SUCCESS;
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
            status = createSurface();
            if (status == BX_FAIL)
            {
                printf("Failed to create window surface!\n");
                assert(BX_FAIL);
            }
        }

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

        if (status == BX_SUCCESS)
        {
            const QueueFamilyIndices& queueFamilyIndices = m_queueMgr.GetHwQueueIndices();
            m_pCmdBufferMgr = std::unique_ptr<Mgr::CmdBufferMgr>(
                new Mgr::CmdBufferMgr(&m_vkDevice, &m_queueMgr, queueFamilyIndices.GetIndexNum()));

            status = m_pCmdBufferMgr->
                createCmdBufferPool(BX_QUEUE_GRAPHICS, queueFamilyIndices.GetQueueFamilyIndex(BX_QUEUE_GRAPHICS));

            if (status == BX_FAIL)
            {
                printf("Failed to created graphics command pool!\n");
                assert(BX_FAIL);
            }
        }

        if (status == BX_SUCCESS)
        {
            m_pTextureMgr = std::unique_ptr<Mgr::TextureMgr>(
                new Mgr::TextureMgr(&(m_vkActiveHwGpuDeviceList[0]),
                    &m_vkDevice, m_pCmdBufferMgr.get(),
                    DEFAULT_MAX_TEXTURE_NUM,
                    m_isSamplerAnisotropySupport));
        }

        if (status == BX_SUCCESS)
        {
            status = createSwapchain();
            if (status == BX_FAIL)
            {
                printf("Failed to created swapchain!\n");
                assert(BX_FAIL);
            }
        }

        if (status == BX_SUCCESS)
        {
            m_pDescriptorMgr = std::unique_ptr<Mgr::DescriptorMgr>(
                new Mgr::DescriptorMgr(&m_vkDevice, m_pCmdBufferMgr.get(), static_cast<UINT>(m_pSwapchainTextures.size())));
        }

        if (status == BX_SUCCESS)
        {
            status = createRender();

            if (status == BX_FAIL)
            {
                printf("Failed to created render!\n");
                assert(BX_FAIL);
            }
        }

        if (status == BX_SUCCESS)
        {
            m_pRender->draw();
        }

        if (status == BX_SUCCESS)
        {
            status = createSemaphores();

            if (status == BX_FAIL)
            {
                printf("Failed to create semaphores!\n");
                assert(BX_FAIL);
            }
        }

        return status;
    }

    BOOL VulkanContext::update(
        const float delta)
    {
        BOOL status = BX_SUCCESS;

        status = m_pRender->update(delta);

        assert(status == BX_SUCCESS);

        return status;
    }

    BOOL VulkanContext::draw()
    {
        BOOL status = BX_SUCCESS;

        // Get the render image index
        UINT     renderImageIndex   = 0;
        VkResult acquireImageResult = vkAcquireNextImageKHR(m_vkDevice,
                                                            m_swapchain,
                                                            std::numeric_limits<UINT64>::max(),
                                                            m_renderSemaphore,
                                                            VK_NULL_HANDLE,
                                                            &renderImageIndex);

        status = VulkanUtility::GetBxStatus(acquireImageResult);
        assert(status == BX_SUCCESS);

        // Submit commands to queue
        if (status == BX_SUCCESS)
        {
            const VkQueue& submitQueue =
                m_queueMgr.GetQueue(m_queueMgr.GetHwQueueIndices().graphicsFamilyIndex).m_queue;

            VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

            // Assumption: every render pass only bind to one command buffer
            const std::vector<Render::VulkanRenderPass>& preDrawPassList = m_pRender->GetPreDrawPassList();
            const UINT preDrawPassNum = static_cast<UINT>(preDrawPassList.size());

            std::vector<VkSemaphore> waitSemaphoreList = { m_renderSemaphore };

            for (UINT preDrawPassIndex = 0;
                 preDrawPassIndex < static_cast<UINT>(preDrawPassList.size());
                 ++preDrawPassIndex)
            {
                const std::vector<UINT>& cmdBufferIndexList =
                    preDrawPassList[preDrawPassIndex].GetCmdBufferIndexList();

                VkSubmitInfo submitInfo         = {};
                submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submitInfo.pWaitDstStageMask    = waitStages;
                submitInfo.commandBufferCount   = 1;
                submitInfo.pCommandBuffers      = m_pCmdBufferMgr->
                    GetCmdBuffer(BX_GRAPHICS_COMMAND_BUFFER, cmdBufferIndexList[0])->GetCmdBufferPtr();

                if (preDrawPassIndex == 0)
                {
                    submitInfo.waitSemaphoreCount   = 1;
                    submitInfo.pWaitSemaphores      = &(waitSemaphoreList[0]);
                    submitInfo.signalSemaphoreCount = 1;
                    submitInfo.pSignalSemaphores    = &(m_preDrawSemaphoreList[0]);
                }
                else
                {
                    submitInfo.waitSemaphoreCount   = 1;
                    submitInfo.pWaitSemaphores      = &(m_preDrawSemaphoreList[preDrawPassIndex - 1]);
                    submitInfo.signalSemaphoreCount = 1;
                    submitInfo.pSignalSemaphores    = &(m_preDrawSemaphoreList[preDrawPassIndex]);
                }

                VkResult submitResult = vkQueueSubmit(submitQueue,
                                                      1,
                                                      &submitInfo,
                                                      VK_NULL_HANDLE);

                status = VulkanUtility::GetBxStatus(submitResult);
                assert(status == BX_SUCCESS);
            }

            VkSubmitInfo submitInfo         = {};
            submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.pWaitDstStageMask    = waitStages;
            submitInfo.waitSemaphoreCount   = 1;
            submitInfo.pWaitSemaphores      = ((preDrawPassList.size() == 0) ? &(waitSemaphoreList[0]) : &(m_preDrawSemaphoreList.back()));
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores    = &m_presentSemaphore;
            submitInfo.commandBufferCount   = 1;
            submitInfo.pCommandBuffers      = m_pCmdBufferMgr->
                GetCmdBuffer(BX_GRAPHICS_COMMAND_BUFFER, renderImageIndex)->GetCmdBufferPtr();

            VkResult submitResult = vkQueueSubmit(submitQueue,
                                                  1,
                                                  &submitInfo,
                                                  VK_NULL_HANDLE);

            status = VulkanUtility::GetBxStatus(submitResult);
            assert(status == BX_SUCCESS);
        }

        // Present
        if (status == BX_SUCCESS)
        {
            const VkQueue& presentQueue    =
                m_queueMgr.GetQueue(m_queueMgr.GetHwQueueIndices().presentSurfaceFamilyIndex).m_queue;

            VkSwapchainKHR swapchain[]     = { m_swapchain };

            VkPresentInfoKHR presentInfo   = {};
            presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.swapchainCount     = 1;
            presentInfo.pSwapchains        = swapchain;
            presentInfo.pImageIndices      = &renderImageIndex;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores    = &m_presentSemaphore;

            VkResult presentResult = vkQueuePresentKHR(presentQueue, &presentInfo);

            status = VulkanUtility::GetBxStatus(acquireImageResult);
            assert(status == BX_SUCCESS);

            VkResult queueWaitResult = vkQueueWaitIdle(presentQueue);
            status = VulkanUtility::GetBxStatus(queueWaitResult);
            assert(status == BX_SUCCESS);
        }

        return status;
    }


    BOOL VulkanContext::createInstance()
    {
        BOOL status = BX_SUCCESS;

        // Checking validation layer support
        if ((m_enableValidationLayer == TRUE) &&
            (VulkanUtility::CheckValidationLayerSupport(m_validationLayers) == FALSE))
        {
            status = FALSE;
        }

        if (status == TRUE)
        {
#if _DEBUG
            std::vector<const char*> extensions = VulkanUtility::GetRequiredExts();
            m_instanceExtCount = static_cast<UINT>(extensions.size());
            m_instanceExtensions = extensions.data();
#else
            m_instanceExtensions = glfwGetRequiredInstanceExtensions(&m_instanceExtCount);
#endif

            VkApplicationInfo appInfo = {};
            appInfo.sType             = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pApplicationName  = m_windowName.data();
            appInfo.pEngineName       = m_engineName.data();
            appInfo.engineVersion     = VK_MAKE_VERSION(1, 0, 0);
            appInfo.apiVersion        = VK_API_VERSION_1_1;

            VkInstanceCreateInfo instanceInfo    = {};
            instanceInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instanceInfo.pApplicationInfo        = &appInfo;
            instanceInfo.enabledExtensionCount   = m_instanceExtCount;
            instanceInfo.ppEnabledExtensionNames = m_instanceExtensions;
            instanceInfo.enabledLayerCount       = static_cast<UINT>(m_validationLayers.size());
            instanceInfo.ppEnabledLayerNames     = m_validationLayers.data();

            VkResult result = vkCreateInstance(&instanceInfo, NULL, m_vkInstance.replace());
            status = ((result == VK_SUCCESS) ? BX_SUCCESS : BX_FAIL);

            assert(status == BX_SUCCESS);
        }

        return status;
    }

    BOOL VulkanContext::createSurface()
    {
        BOOL status = BX_SUCCESS;

        VkResult vkResult = glfwCreateWindowSurface(m_vkInstance, m_pWindow, NULL, m_vkSurface.replace());
        status = ((vkResult == VK_SUCCESS) ? BX_SUCCESS : BX_FAIL);

        assert(status == BX_SUCCESS);

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

            assert(result == BX_SUCCESS);
        }
        else
        {
            assert(BX_FAIL);
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

            // Validate chosen device
            QueueFamilyIndices queueIndices =
                m_queueMgr.retriveHwQueueIndices(m_vkActiveHwGpuDeviceList[0], m_vkSurface);

            m_swapchainHwProperties =
                VulkanUtility::QuerySwapchainHwProperties(m_vkActiveHwGpuDeviceList[0], m_vkSurface);

            if (VulkanUtility::ValidateHwDevice(m_vkActiveHwGpuDeviceList[0],
                                                m_swapchainHwProperties,
                                                queueIndices,
                                                m_deviceExts) == TRUE)
            {
                m_deviceExtSupport = TRUE;

                m_hwProps    = VulkanUtility::GetHwProperties(m_vkActiveHwGpuDeviceList[0]);
                m_hwFeatures = VulkanUtility::GetHwFeatures(m_vkActiveHwGpuDeviceList[0]);

                m_isSamplerAnisotropySupport = m_hwFeatures.samplerAnisotropy;

                result = BX_SUCCESS;
            }
            else
            {
                assert(BX_FAIL);
                result = BX_FAIL;
            }
        }

        return result;
    }

    BOOL VulkanContext::initDevice()
    {
        BOOL result = BX_SUCCESS;

        QueueFamilyIndices queueFamilyIndices    = m_queueMgr.GetHwQueueIndices();
        UINT               queueFamilyIndicesNum = queueFamilyIndices.GetIndexNum();

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfoList;

        std::set<UINT> queueIndicesSet;

        for (UINT i = 0; i < queueFamilyIndicesNum; ++i)
        {
            UINT index = queueFamilyIndices.GetQueueFamilyIndex(i);
            if ((index != -1) &&
                (queueIndicesSet.find(index) == queueIndicesSet.end()))
            {
                VkDeviceQueueCreateInfo queueCreateInfo = {};
                queueCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex        = index;
                queueCreateInfo.queueCount              = 1;

                float priority = m_queueMgr.GetQueue(i).priority;
                queueCreateInfo.pQueuePriorities = &priority;

                queueCreateInfoList.push_back(queueCreateInfo);

                queueIndicesSet.insert(index);
            }
        }

        VkPhysicalDeviceFeatures hwDeviceFeatures = {};
        hwDeviceFeatures.samplerAnisotropy        = m_isSamplerAnisotropySupport;

        VkDeviceCreateInfo deviceCreateInfo   = {};
        deviceCreateInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos    = queueCreateInfoList.data();
        deviceCreateInfo.queueCreateInfoCount = static_cast<UINT>(queueCreateInfoList.size());
        deviceCreateInfo.pEnabledFeatures     = &hwDeviceFeatures;

        if (m_enableValidationLayer == TRUE)
        {
            deviceCreateInfo.enabledLayerCount   = static_cast<UINT>(m_validationLayers.size());
            deviceCreateInfo.ppEnabledLayerNames = m_validationLayers.data();
        }
        else
        {
            deviceCreateInfo.enabledLayerCount = 0;
        }

        if (m_deviceExtSupport == TRUE)
        {
            deviceCreateInfo.enabledExtensionCount   = static_cast<UINT>(m_deviceExts.size());
            deviceCreateInfo.ppEnabledExtensionNames = m_deviceExts.data();
        }
        else
        {
            deviceCreateInfo.enabledExtensionCount   = 0;
            deviceCreateInfo.ppEnabledExtensionNames = NULL;
        }

        VkResult vkResult = vkCreateDevice(m_vkActiveHwGpuDeviceList[0], &deviceCreateInfo, NULL, m_vkDevice.replace());
        result = ((vkResult == VK_SUCCESS) ? BX_SUCCESS : BX_FAIL);

        m_queueMgr.retriveQueueHandle(m_vkDevice);

        return result;
    }

    BOOL VulkanContext::createSwapchain()
    {
        BOOL status = BX_SUCCESS;

        UINT			 swapchainMinImageCount =
            VulkanUtility::GetSwapchainImageCount(m_swapchainHwProperties.m_surfaceCapabilities);

        VkPresentModeKHR swapchainPresentMode =
            VulkanUtility::GetSwapchainPresentMode(m_swapchainHwProperties.m_presentModes,
                                                   BX_SWAPCHAIN_SURFACE_BUFFER_TRIPLE,
                                                   TRUE);
        m_swapchainSurfaceFormat =
            VulkanUtility::GetSwapchainSurfaceFormat(m_swapchainHwProperties.m_surfaceFormats, TRUE);

        m_swapchainExtent =
            VulkanUtility::GetSwapchainExtent(m_swapchainHwProperties.m_surfaceCapabilities,
                                              m_pSetting->resolution.width,
                                              m_pSetting->resolution.height);

        QueueFamilyIndices queueIndices = m_queueMgr.GetHwQueueIndices();

        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
        swapchainCreateInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.oldSwapchain     = VK_NULL_HANDLE; // No previous swapchain
        swapchainCreateInfo.surface          = m_vkSurface;
        swapchainCreateInfo.presentMode      = swapchainPresentMode;
        swapchainCreateInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.imageArrayLayers = 1; // TODO: Stereoscopic 3D app needs to modify this property
        swapchainCreateInfo.imageFormat      = m_swapchainSurfaceFormat.format;
        swapchainCreateInfo.imageColorSpace  = m_swapchainSurfaceFormat.colorSpace;
        swapchainCreateInfo.minImageCount    = swapchainMinImageCount;
        swapchainCreateInfo.imageExtent      = m_swapchainExtent;
        swapchainCreateInfo.preTransform     = m_swapchainHwProperties.m_surfaceCapabilities.currentTransform;
        swapchainCreateInfo.clipped          = VK_TRUE;
        swapchainCreateInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        UINT swapchainQueueIndices[2] =
        {
            static_cast<UINT>(queueIndices.presentSurfaceFamilyIndex),
            static_cast<UINT>(queueIndices.graphicsFamilyIndex)
        };

        if (queueIndices.graphicsFamilyIndex == queueIndices.presentSurfaceFamilyIndex)
        {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }
        else
        {
            swapchainCreateInfo.imageArrayLayers = VK_SHARING_MODE_CONCURRENT;
            swapchainCreateInfo.queueFamilyIndexCount = 2;
            swapchainCreateInfo.pQueueFamilyIndices = &(swapchainQueueIndices[0]);
        }

        VkResult vkResult = vkCreateSwapchainKHR(m_vkDevice, &swapchainCreateInfo, NULL, m_swapchain.replace());
        status = ((vkResult == VK_SUCCESS) ? BX_SUCCESS : BX_FAIL);

        // Retrive the swapchain images
        UINT swapchainImageNum = 0;
        if (status == BX_SUCCESS)
        {
            VkResult vkResult = vkGetSwapchainImagesKHR(m_vkDevice, m_swapchain, &swapchainImageNum, NULL);

            if ((vkResult == VK_SUCCESS) &&
                (swapchainImageNum > 0))
            {
                m_pSwapchainTextures.resize(swapchainImageNum);

                std::vector<VkImage> swapchainImages(swapchainImageNum);
                vkResult = vkGetSwapchainImagesKHR(m_vkDevice, m_swapchain, &swapchainImageNum, swapchainImages.data());

                status = ((vkResult == VK_SUCCESS) ? BX_SUCCESS : BX_FAIL);

                assert(status == BX_SUCCESS);

                // Created textures for swapchain images
                for (UINT i = 0; i < swapchainImageNum; ++i)
                {
                    VDeleter<VkImage> image;
                    *(image.replace()) = swapchainImages[i];

                    Texture::VulkanTexture2D* pBackbufferTex =
                        m_pTextureMgr->createTexture2DRenderTargetFromImage(
                            m_swapchainExtent.width,
                            m_swapchainExtent.height,
                            m_pSetting->m_graphicsSetting.antialasing,
                            Utility::VulkanUtility::GetImageFormat(m_swapchainSurfaceFormat.format),
                            BX_TEXTURE_USAGE_VULKAN_NONE,
                            image,
                            ((m_pSetting->m_graphicsSetting.antialasing == AA_NONE) ? FALSE : TRUE));

                    m_pSwapchainTextures[i] = pBackbufferTex;
                }
            }
            else
            {
                assert(BX_FAIL);
                status = BX_FAIL;
            }
        }
        else
        {
            assert(BX_FAIL);
            status = BX_FAIL;
        }

        return status;
    }

    BOOL VulkanContext::createRender()
    {
        BOOL status = BX_SUCCESS;

        if (m_pSetting->m_graphicsSetting.renderingMethod == FORWARD_RENDERING ||
            m_pSetting->m_graphicsSetting.renderingMethod == FORWARD_PLUS_RENDERING)
        {
            m_pRender = std::unique_ptr<Render::VulkanForwardRender>(
                new Render::VulkanForwardRender(m_pSetting,
                                                &(m_vkActiveHwGpuDeviceList[0]),
                                                &m_vkDevice,
                                                m_pCmdBufferMgr.get(),
                                                m_pDescriptorMgr.get(),
                                                m_pTextureMgr.get(),
                                                m_pRenderSceneList[m_activeSceneIndex],
                                                &m_pSwapchainTextures));
        }
        else if (m_pSetting->m_graphicsSetting.renderingMethod == DEFERRED_RENDERING ||
                 m_pSetting->m_graphicsSetting.renderingMethod == DEFERRED_TILED_RENDERING)
        {
            m_pRender = std::unique_ptr<Render::VulkanDeferredRender>(
                new Render::VulkanDeferredRender(m_pSetting,
                                                 &(m_vkActiveHwGpuDeviceList[0]),
                                                 &m_vkDevice,
                                                 m_pCmdBufferMgr.get(),
                                                 m_pDescriptorMgr.get(),
                                                 m_pTextureMgr.get(),
                                                 m_pRenderSceneList[m_activeSceneIndex],
                                                 &m_pSwapchainTextures));
        }
        else
        {
            NotSupported();
        }

        m_pRender->EnableDepthTest();

        status = m_pRender->initialize();

        return status;
    }

    BOOL VulkanContext::createSemaphores()
    {
        BOOL status = BX_SUCCESS;

        VkSemaphoreCreateInfo semaphoreCreateInfo = {};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreCreateInfo.flags = 0; // For future usage

        VkResult semaphoreCreateResult = vkCreateSemaphore(
            m_vkDevice, &semaphoreCreateInfo, NULL, m_renderSemaphore.replace());

        assert(semaphoreCreateResult == VK_SUCCESS);

        semaphoreCreateResult = vkCreateSemaphore(
            m_vkDevice, &semaphoreCreateInfo, NULL, m_presentSemaphore.replace());

        assert(semaphoreCreateResult == VK_SUCCESS);

        const std::vector<Render::VulkanRenderPass>& preDrawPassList = m_pRender->GetPreDrawPassList();
        const UINT preDrawPassNum = static_cast<UINT>(preDrawPassList.size());

        m_preDrawSemaphoreList.resize(preDrawPassNum, { m_vkDevice, vkDestroySemaphore });
        for (UINT preDrawPassIndex = 0;
            preDrawPassIndex < static_cast<UINT>(preDrawPassList.size());
            ++preDrawPassIndex)
        {
            semaphoreCreateResult = vkCreateSemaphore(
                m_vkDevice, &semaphoreCreateInfo, NULL, m_preDrawSemaphoreList[preDrawPassIndex].replace());

            assert(semaphoreCreateResult == VK_SUCCESS);
        }

        status = VulkanUtility::GetBxStatus(semaphoreCreateResult);

        assert(status == BX_SUCCESS);

        return status;
    }

#if _DEBUG
    BOOL VulkanContext::initDebug()
    {
        BOOL result = BX_SUCCESS;

        VkDebugUtilsMessengerCreateInfoEXT debugMsgCreateInfo = {};
        debugMsgCreateInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugMsgCreateInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
        debugMsgCreateInfo.messageType     =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugMsgCreateInfo.pfnUserCallback = VulkanUtility::debugCallback;
        debugMsgCreateInfo.pUserData = NULL;

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
                callbackInfo.keyboardCallBack[key] = 1;
            }
        }
        else if (action == GLFW_RELEASE)
        {
            callbackInfo.keyboardCallBack[key] = 0;
        }
    }

    void mouse_callback(
        GLFWwindow* window,
        double      x_pos,
        double      y_pos)
    {
        static bool   firstMouseCall = false;
        static double prevPosX       = 0.0;
        static double prevPosY       = 0.0;

        if (!firstMouseCall)
        {
            firstMouseCall = true;
            prevPosX       = x_pos;
            prevPosY       = y_pos;
        }
        else
        {
            callbackInfo.cursorPosCallBack.delta_x = x_pos - prevPosX;
            callbackInfo.cursorPosCallBack.delta_y = y_pos - prevPosY;

            prevPosX = x_pos;
            prevPosY = y_pos;
        }
    }
}