#include "VulkanUtility.h"

BOOL VulkanUtility::CheckValidationLayerSupport(
    const std::vector<const char*>& validationLayers)
{
    BOOL result = TRUE;

#if _DEBUG
    UINT totalLayoutCount = 0;

    vkEnumerateInstanceLayerProperties(&totalLayoutCount, NULL);

    if (totalLayoutCount > 0)
    {
        std::vector<VkLayerProperties> layerProperties(totalLayoutCount);
        vkEnumerateInstanceLayerProperties(&totalLayoutCount, layerProperties.data());

        for (const char* layerName : validationLayers)
        {
            BOOL validLayer = FALSE;

            for (const VkLayerProperties& prop : layerProperties)
            {
                if (strcmp(prop.layerName, layerName) == 0)
                {
                    validLayer = TRUE;
                    break;
                }
            }

            if (validLayer == FALSE)
            {
                result = FALSE;
                break;
            }
        }
    }
#endif

    return result;
}

std::vector<const char*> VulkanUtility::GetRequiredExts()
{
    UINT glfwExtCounts = 0;
    const char** glfwExtNames;

    glfwExtNames = glfwGetRequiredInstanceExtensions(&glfwExtCounts);

    std::vector<const char*> exts(glfwExtNames, glfwExtNames + glfwExtCounts);

    exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return exts;
}

UINT VulkanUtility::GetHwDeviceScore(
    const VkPhysicalDevice& hwGpuDevice)
{
    UINT score = 0;

    VkPhysicalDeviceProperties deviceProps;
    VkPhysicalDeviceFeatures   deviceFeatures;
    vkGetPhysicalDeviceProperties(hwGpuDevice, &deviceProps);
    vkGetPhysicalDeviceFeatures(hwGpuDevice, &deviceFeatures);

    // Heriustic calculate the device score
    if (deviceProps.deviceType ==
        VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        score += 1024;
    }
    else if (deviceProps.deviceType ==
        VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
    {
        score += 64;
    }
    else
    {
        // Not GPU device
        score = 0;
    }

    if (score > 0)
    {
        score += deviceProps.limits.maxImageDimension2D;

        if (deviceFeatures.geometryShader == TRUE)
        {
            score += 128;
        }

        if (deviceFeatures.tessellationShader == TRUE)
        {
            score += 128;
        }

#if _DEBUG
        std::cout << "API version: " << deviceProps.apiVersion << std::endl;
        std::cout << "GPU ID: " << deviceProps.deviceID << std::endl;
        std::cout << "GPU Name: " << deviceProps.deviceName << std::endl;
        std::cout << "Device Type: " << deviceProps.deviceType << std::endl;
        std::cout << "Driver version: " << deviceProps.driverVersion << std::endl;
        std::cout << "Buffer image granularity(size): " <<
            deviceProps.limits.bufferImageGranularity << std::endl;
        std::cout << "Queue priorities: " <<
            deviceProps.limits.discreteQueuePriorities << std::endl;
        std::cout << "Framebuffer Color Sample Counts: " <<
            deviceProps.limits.framebufferColorSampleCounts << std::endl;
        std::cout << "Framebuffer Depth Sample Counts: " <<
            deviceProps.limits.framebufferDepthSampleCounts << std::endl;
        std::cout << "Framebuffer No Attachments SampleCounts: " <<
            deviceProps.limits.framebufferNoAttachmentsSampleCounts << std::endl;
        std::cout << "Framebuffer Stencil Sample Counts: " <<
            deviceProps.limits.framebufferStencilSampleCounts << std::endl;
        std::cout << "Line Width Granularity: " <<
            deviceProps.limits.lineWidthGranularity << std::endl;
        std::cout << "Line Width Range: " <<
            deviceProps.limits.lineWidthRange[0] << " -- " << deviceProps.limits.lineWidthRange[1] << std::endl;
        std::cout << "Max Bound Descriptor Sets: " <<
            deviceProps.limits.maxBoundDescriptorSets << std::endl;
        std::cout << "Max Clip Distance: " <<
            deviceProps.limits.maxClipDistances << std::endl;
        std::cout << "Max Color Attachments" <<
            deviceProps.limits.maxColorAttachments << std::endl;
        std::cout << "Max Combined Clip And Cull Distance: " <<
            deviceProps.limits.maxCombinedClipAndCullDistances << std::endl;

        std::cout << "Max Cull Distance: " <<
            deviceProps.limits.maxCullDistances << std::endl;
        std::cout << "Max Descriptor Set" <<
            deviceProps.limits.maxDescriptorSetInputAttachments << std::endl;
        std::cout << "Max Descriptor Set Sampled Images: " <<
            deviceProps.limits.maxDescriptorSetSampledImages << std::endl;
        std::cout << "Residency aligned mip size: " <<
            deviceProps.sparseProperties.residencyAlignedMipSize << std::endl;

        // Compute properties
        std::cout << "Max Compute Shared Memory: " <<
            deviceProps.limits.maxComputeSharedMemorySize << std::endl;
        std::cout << "Max Compute Work Group: (" <<
            deviceProps.limits.maxComputeWorkGroupCount[0] << ", " <<
            deviceProps.limits.maxComputeWorkGroupCount[1] << ", " <<
            deviceProps.limits.maxComputeWorkGroupCount[2] << ") " << std::endl;
        std::cout << "Max Compute Work Group Invocations:" <<
            deviceProps.limits.maxComputeWorkGroupInvocations << std::endl;
        std::cout << "Max Compute Work Group Size: (" <<
            deviceProps.limits.maxComputeWorkGroupSize[0] << ", " <<
            deviceProps.limits.maxComputeWorkGroupSize[1] << ", " <<
            deviceProps.limits.maxComputeWorkGroupSize[2] << ") " << std::endl;
#endif
    }

    return score;
}

#if _DEBUG

VkResult VulkanUtility::CreateDebugUtilsMessenger(
    VkInstance                                  instance,
    const VkDebugUtilsMessengerCreateInfoEXT*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDebugUtilsMessengerEXT*                   pMessenger)
{
    VkResult result = VK_SUCCESS;

    PFN_vkCreateDebugUtilsMessengerEXT pFunc = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>
                                               (vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

    if (pFunc != NULL)
    {
        result = pFunc(instance, pCreateInfo, pAllocator, pMessenger);
    }
    else
    {
        result = VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    return result;
}

VkResult VulkanUtility::DestroyDebugUtilsMessenger(
    VkInstance                                  instance,
    VkDebugUtilsMessengerEXT                    messenger,
    const VkAllocationCallbacks*                pAllocator)
{
    VkResult result = VK_SUCCESS;

    PFN_vkDestroyDebugUtilsMessengerEXT pFunc = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>
                                                (vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

    if (pFunc != NULL)
    {
        pFunc(instance, messenger, pAllocator);
    }
    else
    {
        result = VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    return result;
}

BOOL VulkanUtility::CheckDeviceExtSupport(
	const VkPhysicalDevice&         hwGpuDevice,
    const std::vector<const char*>& deviceExts)
{
	BOOL result = TRUE;

	UINT extPropsNum = 0;
	vkEnumerateDeviceExtensionProperties(hwGpuDevice, NULL, &extPropsNum, NULL);

	std::vector<VkExtensionProperties> extProps(extPropsNum);
	vkEnumerateDeviceExtensionProperties(hwGpuDevice, NULL, &extPropsNum, extProps.data());

	for (const char* const& deviceExtName : deviceExts)
	{
		BOOL isSupport = FALSE;
		for (const VkExtensionProperties& prop : extProps)
		{
			if (strcmp(prop.extensionName, deviceExtName) == 0)
			{
				isSupport = TRUE;
				break;
			}
		}

		if (isSupport == FALSE)
		{
			result = FALSE;
			break;
		}
	}

	return result;
}

SwapChainHwProperties VulkanUtility::QuerySwapchainHwProperties(
    const VkPhysicalDevice& hwGpuDevice,
    const VkSurfaceKHR&     surface)
{
    SwapChainHwProperties swapChainProperty;

    // Query surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        hwGpuDevice, surface, &(swapChainProperty.m_surfaceCapabilities));

    // Query surface formats
    UINT formatNum = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(hwGpuDevice, surface, &formatNum, NULL);

    if (formatNum > 0)
    {
        swapChainProperty.m_surfaceFormats.resize(static_cast<size_t>(formatNum));
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            hwGpuDevice, surface, &formatNum, swapChainProperty.m_surfaceFormats.data());
    }

    // Query surface present modes
    UINT presentModeNum = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(hwGpuDevice, surface, &presentModeNum, NULL);

    if (presentModeNum > 0)
    {
        swapChainProperty.m_presentModes.resize(presentModeNum);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            hwGpuDevice, surface, &presentModeNum, swapChainProperty.m_presentModes.data());
    }

    return swapChainProperty;
}

BOOL VulkanUtility::ValidateHwDevice(
    const VkPhysicalDevice&         hwGpuDevice,
    const SwapChainHwProperties&    swapChainHwProperties,
    const QueueFamilyIndices&       queueIndices,
    const std::vector<const char*>& deviceExts)
{
    BOOL result = TRUE;
    
    BOOL isQueueComplete    = queueIndices.IsCompleted();
    BOOL isDeviceExtSupport = CheckDeviceExtSupport(hwGpuDevice, deviceExts);

    BOOL isSwapchainSupport = ((swapChainHwProperties.m_surfaceFormats.empty() == FALSE) &&
                               (swapChainHwProperties.m_presentModes.empty()   == FALSE));

    result = ((isQueueComplete    == TRUE) &&
              (isDeviceExtSupport == TRUE) &&
              (isSwapchainSupport == TRUE));

    return result;
}

VkExtent2D VulkanUtility::GetSwapchainExtent(
    const VkSurfaceCapabilitiesKHR& surfaceCapabilities,
    const UINT&                     windowWidth,
    const UINT&                     windowHeight)
{
    VkExtent2D swapchainExtent;

    swapchainExtent.width = std::max(surfaceCapabilities.minImageExtent.width,
                            std::min(surfaceCapabilities.maxImageExtent.width, windowWidth));

    swapchainExtent.height = std::max(surfaceCapabilities.minImageExtent.height,
                             std::min(surfaceCapabilities.maxImageExtent.height, windowHeight));

    return swapchainExtent;
}

VkSurfaceFormatKHR VulkanUtility::GetSwapchainSurfaceFormat(
	const std::vector<VkSurfaceFormatKHR>& surfaceFormats,
	const BOOL							   linearFormat)
{
	VkSurfaceFormatKHR swapchainSurfaceFormat = {};

	VkFormat targetFormat = ((linearFormat == TRUE) ? VK_FORMAT_B8G8R8A8_UNORM : VK_FORMAT_B8G8R8A8_SNORM);

	if ((surfaceFormats.size()	  == 1) &&
		(surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
	{
		swapchainSurfaceFormat.format	  = targetFormat;
		swapchainSurfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	}
	else
	{
		for (const VkSurfaceFormatKHR& surfaceFormat : surfaceFormats)
		{
			if ((surfaceFormat.format     == targetFormat) &&
				(surfaceFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR))
			{
				swapchainSurfaceFormat = surfaceFormat;
				break;
			}
		}
	}

	return swapchainSurfaceFormat;
}

VkPresentModeKHR VulkanUtility::GetSwapchainPresentMode(
	const std::vector<VkPresentModeKHR>& presentModes,
	BX_SWAPCHAIN_SURFACE_BUFFER		     swapchainSurfaceBuffer,
	BOOL								 vSync)
{
	VkPresentModeKHR swapchainPresentMode;

	switch (swapchainSurfaceBuffer)
	{
	case BX_SWAPCHAIN_SURFACE_BUFFER_SINGLE:
	{
		if (vSync == FALSE)
		{
			swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		}
		break;
	}
	case BX_SWAPCHAIN_SURFACE_BUFFER_DOUBLE:
	{
		if (vSync == TRUE)
		{
			swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		}
		else
		{
			swapchainPresentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
		}
		break;
	}
	case BX_SWAPCHAIN_SURFACE_BUFFER_TRIPLE:
	{
		swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		break;
	}
	default:
		printf("Not support present mode, use default FIFO mode!\n");
		swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		break;
	}

	BOOL presentModeExist = FALSE;
	for (const VkPresentModeKHR& presentMode : presentModes)
	{
		if (presentMode == swapchainPresentMode)
		{
			presentModeExist = TRUE;
			break;
		}
	}

	// The present mode is not supported, use the default FIFO mode
	if (presentModeExist == FALSE)
	{
		swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	}

	return swapchainPresentMode;
}

UINT VulkanUtility::GetSwapchainImageCount(
    const VkSurfaceCapabilitiesKHR& surfaceCapabilities)
{
    UINT imageCount = surfaceCapabilities.minImageCount + 1;

    imageCount = ((surfaceCapabilities.maxImageCount > 0) ?
        std::min(surfaceCapabilities.maxImageCount, imageCount) : imageCount);

    return imageCount;
}

std::vector<char> VulkanUtility::ReadFile(
    const std::string & fileName,
    const BOOL          isBinary)
{
    std::vector<char> fileString;

    INT binaryBit = ((isBinary == TRUE) ? std::ios::binary : 0);
    std::ifstream fileStream(fileName, std::ios::ate | binaryBit);

    if (fileStream.is_open() == TRUE)
    {
        UINT fileSize = static_cast<UINT>(fileStream.tellg());
        fileString.resize(fileSize);

        fileStream.seekg(0);
        fileStream.read(fileString.data(), fileSize);
    }
    else
    {
        printf("Failed to open file!\n");
        assert(FALSE);
    }

    fileStream.close();

    return fileString;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanUtility::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT		severiry,
	VkDebugUtilsMessageTypeFlagsEXT				type,
	const VkDebugUtilsMessengerCallbackDataEXT* pData,
	void*										pUserData)
{
	std::cerr << "BxEngine validation layer: " << pData->pMessage << std::endl;

	return VK_FALSE;
}

#endif