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
        const Setting* pSetting)
        : m_pSetting(pSetting),
        m_pWindow(NULL),
        m_windowName("BxEngine Vulkan"),
        m_engineName("BxEngine"),
        m_prevTime(0.0f),
        m_deltaTime(0.0f),
        m_instanceExtCount(0),
        m_instanceExtensions(NULL),
        m_deviceExtSupport(FALSE)
    {
        // Set resource release callback functions
        m_vkInstance             = { vkDestroyInstance };
        m_vkSurface              = { m_vkInstance, vkDestroySurfaceKHR };
        m_vkDevice               = { vkDestroyDevice };
        m_swapchain              = { m_vkDevice, vkDestroySwapchainKHR };
        m_graphicsPipelineLayout = { m_vkDevice, vkDestroyPipelineLayout };
        m_renderPass             = { m_vkDevice, vkDestroyRenderPass };
        m_graphicsPipeline       = { m_vkDevice, vkDestroyPipeline };
        m_renderSemaphore        = { m_vkDevice, vkDestroySemaphore };
        m_presentSemaphore       = { m_vkDevice, vkDestroySemaphore };

#if _DEBUG
        m_vkDebugMsg             = { m_vkInstance, VulkanUtility::DestroyDebugUtilsMessenger };
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
        BOOL status = BX_SUCCESS;

        while (glfwWindowShouldClose(m_pWindow) == FALSE)
        {
            glfwSwapBuffers(m_pWindow);
            glfwPollEvents();

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
            status = createSwapchain();
            if (status == BX_FAIL)
            {
                printf("Failed to created swapchain!\n");
                assert(BX_FAIL);
            }
        }

        if (status = BX_SUCCESS)
        {
            status = createGraphicsPipeline();

            if (status == BX_FAIL)
            {
                printf("Failed to created graphics pipeline!\n");
                assert(BX_FAIL);
            }
        }

        if (status = BX_SUCCESS)
        {
            status = createSwapchainFramebuffer();;

            if (status == BX_FAIL)
            {
                printf("Failed to created swapchain framebuffer!\n");
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
            status = m_pCmdBufferMgr->
                addGraphicsCmdBuffers(BX_QUEUE_GRAPHICS,
                    BX_DIRECT_COMMAND_BUFFER,
                    static_cast<UINT>(m_swapchainFramebuffers.size()));

            if (status == BX_FAIL)
            {
                printf("Failed to create graphics command buffers!\n");
                assert(BX_FAIL);
            }

            for (size_t i = 0; i < m_swapchainFramebuffers.size(); ++i)
            {
                // Start recording to command buffer
                Buffer::CmdBuffer* const pCmdBuffer = m_pCmdBufferMgr->
                    GetCmdBuffer(BX_GRAPHICS_COMMAND_BUFFER, static_cast<UINT>(i));

                status = pCmdBuffer->beginCmdBuffer(TRUE);

                // Start render pass, write draw commands
                if (status == BX_SUCCESS)
                {
                    // TODO: Remove the hard code here
                    VkRect2D renderArea = { { 0, 0 }, m_swapchainExtent };
                    std::vector<VkClearValue> clearColorValue = { { 0.0f, 0.0f, 0.0f, 1.0f } };

                    pCmdBuffer->beginRenderPass(m_renderPass,
                        m_swapchainFramebuffers[i],
                        renderArea,
                        clearColorValue);

                    pCmdBuffer->cmdDrawArrays(m_graphicsPipeline, 3, 0);

                    pCmdBuffer->endRenderPass();
                }
                else
                {
                    printf("Failed to begin graphics command buffers!\n");
                    assert(BX_FAIL);
                }

                // Finish recording to command buffer
                status = pCmdBuffer->endCmdBuffer();

                if (status == BX_FAIL)
                {
                    printf("Failed to end graphics command buffers!\n");
                    assert(BX_FAIL);
                }
            }
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
            VkSemaphore          waitSemaphore[] = { m_renderSemaphore };
            VkSemaphore          signalSemaphore[] = { m_presentSemaphore };

            VkSubmitInfo submitInfo         = {};
            submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.pWaitDstStageMask    = waitStages;
            submitInfo.waitSemaphoreCount   = 1;
            submitInfo.pWaitSemaphores      = waitSemaphore;
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores    = signalSemaphore;
            submitInfo.commandBufferCount   = 1;
            submitInfo.pCommandBuffers      = m_pCmdBufferMgr->
                GetCmdBuffer(BX_GRAPHICS_COMMAND_BUFFER, renderImageIndex)->GetCmdBufferPtr();

            VkResult submitResult = vkQueueSubmit(
                submitQueue,
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

            VkSemaphore    waitSemaphore[] = { m_presentSemaphore };
            VkSwapchainKHR swapchain[]     = { m_swapchain };

            VkPresentInfoKHR presentInfo   = {};
            presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.swapchainCount     = 1;
            presentInfo.pSwapchains        = swapchain;
            presentInfo.pImageIndices      = &renderImageIndex;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores    = waitSemaphore;

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
        swapchainCreateInfo.clipped = VK_TRUE;
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
                m_swapchainImages.resize(swapchainImageNum);
                vkResult = vkGetSwapchainImagesKHR(m_vkDevice, m_swapchain, &swapchainImageNum, m_swapchainImages.data());

                status = ((vkResult == VK_SUCCESS) ? BX_SUCCESS : BX_FAIL);
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

        // Create Image views for the swapchain images
        if (status == BX_SUCCESS)
        {
            m_swapchainImagesViews.resize(swapchainImageNum, { m_vkDevice, vkDestroyImageView });
            m_swapchainFramebuffers.resize(swapchainImageNum, { m_vkDevice, vkDestroyFramebuffer });

            for (UINT i = 0; i < swapchainImageNum; ++i)
            {
                VkImageViewCreateInfo swapchainImageViewCreateInfo = {};
                swapchainImageViewCreateInfo.sType        = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                swapchainImageViewCreateInfo.viewType     = VK_IMAGE_VIEW_TYPE_2D;
                swapchainImageViewCreateInfo.image        = m_swapchainImages[i];
                swapchainImageViewCreateInfo.format       = m_swapchainSurfaceFormat.format;
                swapchainImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
                swapchainImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
                swapchainImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
                swapchainImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;

                swapchainImageViewCreateInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                swapchainImageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
                swapchainImageViewCreateInfo.subresourceRange.levelCount     = 1;
                swapchainImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
                swapchainImageViewCreateInfo.subresourceRange.layerCount     = 1;

                VkResult vkResult = vkCreateImageView(
                    m_vkDevice, &swapchainImageViewCreateInfo, NULL, m_swapchainImagesViews[i].replace());
                if (vkResult != VK_SUCCESS)
                {
                    assert(BX_FAIL);
                    status = BX_FAIL;
                    break;
                }
            }
        }

        return status;
    }

    BOOL VulkanContext::createGraphicsPipeline()
    {
        BOOL status = BX_SUCCESS;

        /// Setup Fixed pipeline stages
        // VS input
        VkPipelineVertexInputStateCreateInfo vsInputCreateInfo = {};
        vsInputCreateInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vsInputCreateInfo.vertexBindingDescriptionCount   = 0;
        vsInputCreateInfo.pVertexBindingDescriptions      = NULL;
        vsInputCreateInfo.vertexAttributeDescriptionCount = 0;
        vsInputCreateInfo.pVertexAttributeDescriptions    = NULL;

        // Input assembly state
        VkPipelineInputAssemblyStateCreateInfo inputAsmCreateInfo = {};
        inputAsmCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAsmCreateInfo.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAsmCreateInfo.primitiveRestartEnable = VK_FALSE;

        // Rasterizer
        VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {};
        rasterizerCreateInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizerCreateInfo.depthClampEnable        = VK_FALSE;
        rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizerCreateInfo.polygonMode             = VulkanUtility::GetVkPolygonMode(m_pSetting->polyMode);
        rasterizerCreateInfo.lineWidth               = 1.0f;
        rasterizerCreateInfo.cullMode                = VK_CULL_MODE_BACK_BIT;
        rasterizerCreateInfo.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizerCreateInfo.depthBiasEnable         = VK_FALSE;

        // Multisampling
        VkPipelineMultisampleStateCreateInfo multiSamplingCreateInfo = {};
        multiSamplingCreateInfo.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multiSamplingCreateInfo.rasterizationSamples = VulkanUtility::GetVkSampleCount(m_pSetting->m_graphicsSetting.antialasing);
        multiSamplingCreateInfo.sampleShadingEnable  = VK_FALSE;

        // Depth/Stencil
        VkPipelineColorBlendAttachmentState blendAttachmentState = {};
        blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        if (m_pSetting->m_graphicsSetting.blend == TRUE)
        {
            blendAttachmentState.blendEnable         = VK_TRUE;
            blendAttachmentState.alphaBlendOp        = VK_BLEND_OP_ADD;
            blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            blendAttachmentState.alphaBlendOp        = VK_BLEND_OP_ADD;
            blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        }
        else
        {
            blendAttachmentState.blendEnable = VK_FALSE;
        }

        VkPipelineColorBlendStateCreateInfo blendState = {};
        blendState.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        blendState.attachmentCount = 1;
        blendState.pAttachments    = &blendAttachmentState;
        blendState.logicOpEnable   = VK_FALSE; // Enable will disable the states in pAttachments

        // Viewport and scissor
        VkViewport viewport = {};
        viewport.x          = 0.0f;
        viewport.y          = 0.0f;
        viewport.width      = static_cast<float>(m_swapchainExtent.width);
        viewport.height     = static_cast<float>(m_swapchainExtent.height);
        viewport.minDepth   = 0.0f;
        viewport.maxDepth   = 1.0f;

        VkRect2D scissor = {};
        scissor.extent   = m_swapchainExtent;
        scissor.offset   = { 0, 0 };

        VkPipelineViewportStateCreateInfo viewportCreateInfo = {};
        viewportCreateInfo.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportCreateInfo.viewportCount = 1;
        viewportCreateInfo.pViewports    = &viewport;
        viewportCreateInfo.scissorCount  = 1;
        viewportCreateInfo.pScissors     = &scissor;

        // Pipeline Layout
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

        VkResult vkResult = vkCreatePipelineLayout(
            m_vkDevice, &pipelineLayoutCreateInfo, NULL, m_graphicsPipelineLayout.replace());

        status = ((vkResult == VK_SUCCESS) ? BX_SUCCESS : BX_FAIL);
        assert(status == BX_SUCCESS);

        // Render pass
        VkAttachmentDescription colorAttachement = {};
        colorAttachement.format         = m_swapchainSurfaceFormat.format;
        colorAttachement.samples        = VulkanUtility::GetVkSampleCount(m_pSetting->m_graphicsSetting.antialasing);
        colorAttachement.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachement.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachement.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachement.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachement.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachement.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment            = 0; // Output layout index in shader,
                                                      // e.g. layout(location = 0) out vec4 outColor
        colorAttachmentRef.layout                = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDescription = {};
        subpassDescription.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.colorAttachmentCount = 1;
        subpassDescription.pColorAttachments    = &colorAttachmentRef;

        VkRenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount        = 1;
        renderPassCreateInfo.pAttachments           = &colorAttachement;
        renderPassCreateInfo.subpassCount           = 1;
        renderPassCreateInfo.pSubpasses             = &subpassDescription;

        VkResult createRenderpassResult = vkCreateRenderPass(m_vkDevice, &renderPassCreateInfo, NULL, m_renderPass.replace());
        status = ((createRenderpassResult == VK_SUCCESS) ? BX_SUCCESS : BX_FAIL);

        assert(status == BX_SUCCESS);

        /// End fixed pipeline stages setup

        /// Setup programmable pipeline stages
        std::vector<VkPipelineShaderStageCreateInfo> shaderCreateInfo;
        Shader::BxShaderMeta                         shaderMeta;
        if (status == BX_SUCCESS)
        {
            m_pShader                                = 
                std::unique_ptr<Shader::VulkanGraphicsShader>(
                    new Shader::VulkanGraphicsShader(m_vkDevice));
            shaderMeta.vertexShaderInfo.shaderFile   = "SimpleTriangle.vert.spv";
            shaderMeta.fragmentShaderInfo.shaderFile = "SimpleTriangle.frag.spv";
            shaderCreateInfo                         =
                m_pShader->createPipelineShaderStages(shaderMeta);
        }
        /// End programmable pipeline stages setup

        // Create pipeline
        if (status == BX_SUCCESS)
        {
            VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
            graphicsPipelineCreateInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            graphicsPipelineCreateInfo.pVertexInputState   = &vsInputCreateInfo;
            graphicsPipelineCreateInfo.pInputAssemblyState = &inputAsmCreateInfo;
            graphicsPipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
            graphicsPipelineCreateInfo.pMultisampleState   = &multiSamplingCreateInfo;
            graphicsPipelineCreateInfo.pColorBlendState    = &blendState;
            graphicsPipelineCreateInfo.pDepthStencilState  = NULL;
            graphicsPipelineCreateInfo.pViewportState      = &viewportCreateInfo;
            graphicsPipelineCreateInfo.layout              = m_graphicsPipelineLayout;
            graphicsPipelineCreateInfo.renderPass          = m_renderPass;
            graphicsPipelineCreateInfo.stageCount          = static_cast<UINT>(shaderCreateInfo.size());
            graphicsPipelineCreateInfo.pStages             = shaderCreateInfo.data();
            graphicsPipelineCreateInfo.pDynamicState       = NULL;
            graphicsPipelineCreateInfo.subpass             = 0;

            VkResult graphicsPipelineCreateResult = vkCreateGraphicsPipelines(m_vkDevice,
                VK_NULL_HANDLE,
                1,
                &graphicsPipelineCreateInfo,
                NULL,
                m_graphicsPipeline.replace());

            status = ((graphicsPipelineCreateResult == VK_SUCCESS) ? BX_SUCCESS : BX_FAIL);

            assert(status == BX_SUCCESS);
        }

        return status;
    }

    BOOL VulkanContext::createSwapchainFramebuffer()
    {
        BOOL status = BX_SUCCESS;

        for (size_t i = 0; i < m_swapchainImagesViews.size(); ++i)
        {
            VkImageView imageViews[] = { m_swapchainImagesViews[i] };

            VkFramebufferCreateInfo swapchainFramebufferCreateInfo = {};
            swapchainFramebufferCreateInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            swapchainFramebufferCreateInfo.renderPass      = m_renderPass;
            swapchainFramebufferCreateInfo.attachmentCount = 1;
            swapchainFramebufferCreateInfo.pAttachments    = imageViews;
            swapchainFramebufferCreateInfo.width           = m_swapchainExtent.width;
            swapchainFramebufferCreateInfo.height          = m_swapchainExtent.height;
            swapchainFramebufferCreateInfo.layers          = 1;

            VkResult createSwapchainFramebufferResult = vkCreateFramebuffer(
                m_vkDevice, &swapchainFramebufferCreateInfo, NULL, m_swapchainFramebuffers[i].replace());

            status = ((createSwapchainFramebufferResult == VK_SUCCESS) ? BX_SUCCESS : BX_FAIL);

            assert(status == BX_SUCCESS);
        }

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
}