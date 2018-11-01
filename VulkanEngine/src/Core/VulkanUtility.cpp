#include "VulkanUtility.h"

const std::vector<const char*> VulkanUtility::m_validationLayers =
{
    "VK_LAYER_LUNARG_standard_validation"
};

BOOL VulkanUtility::CheckValidationLayerSupport(
    UINT*               layerCount,
    const char* const** pLayerNames)
{
    BOOL result = TRUE;

#if _DEBUG
    UINT totalLayoutCount = 0;

    vkEnumerateInstanceLayerProperties(&totalLayoutCount, NULL);

    if (totalLayoutCount > 0)
    {
        std::vector<VkLayerProperties> layerProperties(totalLayoutCount);
        vkEnumerateInstanceLayerProperties(&totalLayoutCount, layerProperties.data());

        for (const char* layerName : m_validationLayers)
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

        if (result == TRUE)
        {
            *pLayerNames = m_validationLayers.data();
            *layerCount  = static_cast<UINT>(m_validationLayers.size());
        }
        else
        {
            *pLayerNames = NULL;
            *layerCount  = 0;
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