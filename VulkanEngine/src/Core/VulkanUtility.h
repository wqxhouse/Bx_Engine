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

    static inline BOOL IsValidationLayerEnabled() { return m_enableValidationLayer; }

private:
    const static std::vector<const char*> m_validationLayers;

#if _DEBUG
    const static BOOL m_enableValidationLayer = TRUE;
#else
    const static bool m_enableValidationLayer = FALSE;
#endif
};
