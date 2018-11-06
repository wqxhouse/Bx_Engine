#pragma once

#include "VulkanPCH.h"
#include "Utility.h"

#include "../Context/BxQueue.h"

struct SwapChainHwProperties
{
    VkSurfaceCapabilitiesKHR        m_surfaceCapabilities;
    std::vector<VkSurfaceFormatKHR> m_surfaceFormats;
    std::vector<VkPresentModeKHR>   m_presentModes;
};

class VulkanUtility
{
public:
    static BOOL CheckValidationLayerSupport(
        const std::vector<const char*>& validationLayers);

    static std::vector<const char*> GetRequiredExts();

    static UINT GetHwDeviceScore(
        const VkPhysicalDevice& hwGpuDevice);

    static SwapChainHwProperties QuerySwapchainHwProperties(
        const VkPhysicalDevice& hwGpuDevice,
        const VkSurfaceKHR&     surface);

    static BOOL CheckDeviceExtSupport(
        const VkPhysicalDevice&         hwGpuDevice,
        const std::vector<const char*>& deviceExts);

    static BOOL ValidateHwDevice(
        const VkPhysicalDevice&         hwGpuDevice,
        const VkSurfaceKHR&             surface,
        const QueueFamilyIndices&       queueIndices,
        const std::vector<const char*>& deviceExts);

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
};
