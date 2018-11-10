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
        const SwapChainHwProperties&    swapChainHwProperties,
        const QueueFamilyIndices&       queueIndices,
        const std::vector<const char*>& deviceExts);

    static VkExtent2D GetSwapchainExtent(
        const VkSurfaceCapabilitiesKHR& surfaceCapabilities,
        const UINT&                     windowWidth,
        const UINT&                     windowHeight);

	static VkSurfaceFormatKHR GetSwapchainSurfaceFormat(
		const std::vector<VkSurfaceFormatKHR>& surfaceFormats,
		const BOOL							   linearFormat);

	static VkPresentModeKHR GetSwapchainPresentMode(
		const std::vector<VkPresentModeKHR>& presentModes,
		BX_SWAPCHAIN_SURFACE_BUFFER		     swapchainSurfaceBuffer,
		BOOL								 vSync);

    static UINT GetSwapchainImageCount(
        const VkSurfaceCapabilitiesKHR& surfaceCapabilities);

    static std::vector<char> ReadFile(
        const std::string& fileName,
        const BOOL         isBinary);

    static inline VkPolygonMode GetVkPolygonMode(
        const PolyMode polyMode)
    {
        VkPolygonMode vkPolyMode = VK_POLYGON_MODE_BEGIN_RANGE;

        switch (polyMode)
        {
            case POLYMODE_POINT:
                vkPolyMode = VK_POLYGON_MODE_POINT;
                break;
            case POLYMODE_WIREFRAME:
                vkPolyMode = VK_POLYGON_MODE_LINE;
                break;
            case POLYMODE_FILL:
                vkPolyMode = VK_POLYGON_MODE_FILL;
                break;
            default:
                break;
        }

        return vkPolyMode;
    }

    static inline VkSampleCountFlagBits GetSampleCount(
        const UINT sampleCount)
    {
        VkSampleCountFlagBits sampleCountFlagBit = VK_SAMPLE_COUNT_1_BIT;

        switch (sampleCount)
        {
            case 1:
                sampleCountFlagBit = VK_SAMPLE_COUNT_1_BIT;
                break;
            case 2:
                sampleCountFlagBit = VK_SAMPLE_COUNT_2_BIT;
                break;
            case 4:
                sampleCountFlagBit = VK_SAMPLE_COUNT_4_BIT;
                break;
            case 8:
                sampleCountFlagBit = VK_SAMPLE_COUNT_8_BIT;
                break;
            case 16:
                sampleCountFlagBit = VK_SAMPLE_COUNT_16_BIT;
                break;
            case 32:
                sampleCountFlagBit = VK_SAMPLE_COUNT_32_BIT;
                break;
            case 64:
                sampleCountFlagBit = VK_SAMPLE_COUNT_64_BIT;
                break;
            default:
                break;
        }

        return sampleCountFlagBit;
    }

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
