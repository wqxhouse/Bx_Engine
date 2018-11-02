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