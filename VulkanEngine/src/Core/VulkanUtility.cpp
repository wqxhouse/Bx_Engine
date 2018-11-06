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
    const VkSurfaceKHR&             surface,
    const QueueFamilyIndices&       queueIndices,
    const std::vector<const char*>& deviceExts)
{
    BOOL result = TRUE;
    
    BOOL isQueueComplete    = queueIndices.IsCompleted();
    BOOL isDeviceExtSupport = CheckDeviceExtSupport(hwGpuDevice, deviceExts);

    SwapChainHwProperties swapChainHwProperties = QuerySwapchainHwProperties(hwGpuDevice, surface);
    BOOL isSwapchainSupport = ((swapChainHwProperties.m_surfaceFormats.empty() == FALSE) &&
                               (swapChainHwProperties.m_presentModes.empty()   == FALSE));

    result = ((isQueueComplete    == TRUE) &&
              (isDeviceExtSupport == TRUE) &&
              (isSwapchainSupport == TRUE));

    return result;
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