#include "VulkanUtility.h"

const std::vector<const char*> VulkanUtility::m_validationLayers =
{
    "VK_LAYER_LUNARG_standard_validation"
};

BOOL VulkanUtility::CheckValidationLayerSupport()
{
    BOOL result = TRUE;

    UINT layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);

    std::vector<VkLayerProperties> layerProperties(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

    for(const char* layerName : m_validationLayers)
    {
        if (result == TRUE)
        {
            for(const VkLayerProperties& prop : layerProperties)
            {
                if(strcmp(prop.layerName, layerName) != 0)
                {
                    result = FALSE;
                    break;
                }
            }
        }
        else
        {
            break;
        }
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

std::vector<const char*> VulkanUtility::getRequiredExts()
{
	UINT glfwExtCounts = 0;
	const char** glfwExtNames;

	glfwExtNames = glfwGetRequiredInstanceExtensions(&glfwExtCounts);

	std::vector<const char*> exts(glfwExtNames, glfwExtNames + glfwExtCounts);

	exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return exts;
}
