#pragma once

#include "VulkanPCH.h"
#include "Utility.h"

class VulkanUtility
{
public:
    static BOOL CheckValidationLayerSupport(
        UINT*               layerCount,
        const char* const** pLayerNames);

    static std::vector<const char*> GetRequiredExts();

    static UINT GetHwDeviceScore(
        const VkPhysicalDevice& hwGpuDevice);

#if _DEBUG
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT		severiry,
        VkDebugUtilsMessageTypeFlagsEXT				type,
        const VkDebugUtilsMessengerCallbackDataEXT* pData,
        void*										pUserData);

    static VkResult CreateDebugUtilsMessenger(
        VkInstance                                  instance,
        const VkDebugUtilsMessengerCreateInfoEXT*   pCreateInfo,
        const VkAllocationCallbacks*                pAllocator,
        VkDebugUtilsMessengerEXT*                   pMessenger);

    static VkResult DestroyDebugUtilsMessenger(
        VkInstance                                  instance,
        VkDebugUtilsMessengerEXT                    messenger,
        const VkAllocationCallbacks*                pAllocator);
#endif

    const static std::vector<const char*> m_validationLayers;
};
