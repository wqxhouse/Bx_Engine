//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "VulkanPCH.h"
#include <Core/Utility.h>

#include "../Context/BxQueue.h"

namespace VulkanEngine
{
    namespace Utility
    {
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
                const VkPhysicalDevice&          hwGpuDevice,
                const SwapChainHwProperties&     swapChainHwProperties,
                const Queue::QueueFamilyIndices& queueIndices,
                const std::vector<const char*>&  deviceExts);

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

            static UINT FindMemoryType(
                const VkPhysicalDeviceMemoryProperties& hwMemProps,
                const UINT                              filter,
                const VkMemoryPropertyFlags&            props);

            static INLINE VkPolygonMode GetVkPolygonMode(
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

            static INLINE VkSampleCountFlagBits GetVkSampleCount(
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

            static INLINE INT GetQueueCmdPoolIndex(
                const VkQueueFlags queueFlag)
            {
                INT queueCmdPoolIndex = -1;

                switch (queueFlag)
                {
                case VK_QUEUE_GRAPHICS_BIT:
                    queueCmdPoolIndex = BX_QUEUE_GRAPHICS;
                    break;
                case VK_QUEUE_COMPUTE_BIT:
                    queueCmdPoolIndex = BX_QUEUE_COMPUTE;
                    break;
                default:
                    assert(FALSE);
                    break;
                }

                assert(queueCmdPoolIndex != -1);

                return queueCmdPoolIndex;
            }

            static INLINE VkCommandBufferLevel GetVkCmdBufferLevel(
                const BX_COMMAND_BUFFER_LEVLE cmdBufferLevel)
            {
                VkCommandBufferLevel vkCmdBufferLevel;

                switch (cmdBufferLevel)
                {
                case BX_DIRECT_COMMAND_BUFFER:
                    vkCmdBufferLevel = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                    break;
                case BX_INDIRECT_COMMAND_BUFFER:
                    vkCmdBufferLevel = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
                    break;
                default:
                    assert(FALSE);
                    break;
                }

                return vkCmdBufferLevel;
            }

            static INLINE VkVertexInputRate GetVkVertexInputRate(
                const BX_VERTEX_INPUT_RATE rate)
            {
                VkVertexInputRate inputRate;

                switch (rate)
                {
                case BX_VERTEX_INPUT_RATE_VERTEX:
                    inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                    break;
                case BX_VERTEX_INPUT_RATE_INSTANCE:
                    inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
                    break;
                default:
                    assert(FALSE);
                    break;
                }

                return inputRate;
            }

            static INLINE VkIndexType GetVkIndexType(
                const BX_INDEX_TYPE type)
            {
                VkIndexType indexType;

                switch (type)
                {
                    case BX_INDEX_UINT16:
                        indexType = VK_INDEX_TYPE_UINT16;
                        break;
                    case BX_INDEX_UINT32:
                        indexType = VK_INDEX_TYPE_UINT32;
                        break;
                    default:
                        assert(FALSE);
                        break;
                }

                return indexType;
            }

            static INLINE VkDescriptorType GetVkDescriptorType(
                const BX_DESCRIPTOR_TYPE type)
            {
                VkDescriptorType descriptorType;

                switch (type)
                {
                    case BX_UNIFORM_DESCRIPTOR:
                        descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        break;
                    case BX_STORAGE_BUFFER_DESCRIPTOR:
                        descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                        break;
                    default:
                        assert(FALSE);
                        break;
                }

                return descriptorType;
            }

            static INLINE VkFormat GetVkImageFormat(TextureFormat format)
            {
                VkFormat imageFormat;

                switch (format)
                {
                    case BX_FORMAT_R8:
                        imageFormat = VK_FORMAT_R8_UNORM;
                        break;
                    case BX_FORMAT_RG8:
                        imageFormat = VK_FORMAT_R8G8_UNORM;
                        break;
                    case BX_FORMAT_RGB8:
                        imageFormat = VK_FORMAT_R8G8B8_UNORM;
                        break;
                    case BX_FORMAT_RGBA8:
                        imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
                        break;
                    case BX_FORMAT_RGBA16:
                        imageFormat = VK_FORMAT_R16G16B16A16_UNORM;
                        break;
                    case BX_FORMAT_RGBA32:
                        imageFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
                        break;
                    case BX_FORMAT_RGBA64:
                        imageFormat = VK_FORMAT_R64G64B64A64_SFLOAT;
                        break;
                    case BX_FORMAT_RGBA:
                        imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
                        break;
                    case BX_FORMAT_SRGB:
                        imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
                        break;
                    case BX_FORMAT_DEPTH16:
                        imageFormat = VK_FORMAT_D16_UNORM;
                        break;
                    case BX_FORMAT_DEPTH16_STENCIL:
                        imageFormat = VK_FORMAT_D16_UNORM_S8_UINT;
                        break;
                    case BX_FORMAT_DEPTH24_STENCIL:
                        imageFormat = VK_FORMAT_D24_UNORM_S8_UINT;
                        break;
                    case BX_FORMAT_DEPTH32:
                        imageFormat = VK_FORMAT_D32_SFLOAT;
                        break;
                    case BX_FORMAT_DEPTH32_STENCIL:
                        imageFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;
                        break;
                    default:
                        assert(FALSE);
                        break;
                }

                return imageFormat;
            }

            static INLINE VkImageUsageFlags GetVkImageUsage(TextureUsage texUsage)
            {
                VkImageUsageFlags imageUsageFlagBits;

                if ((texUsage | BX_TEXTURE_USAGE_SAMPLED) != 0)
                {
                    imageUsageFlagBits |= VK_IMAGE_USAGE_SAMPLED_BIT;
                }

                if ((texUsage | BX_TEXTURE_USAGE_RENDER_TARGET) != 0)
                {
                    imageUsageFlagBits |= VK_IMAGE_USAGE_STORAGE_BIT;
                }

                if ((texUsage | BX_TEXTURE_USAGE_COLOR_ATTACHMENT) != 0)
                {
                    imageUsageFlagBits |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                }

                if ((texUsage | BX_TEXTURE_USAGE_DEPTH_STENCIL_ATTACHMENT) != 0)
                {
                    imageUsageFlagBits |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                }

                return imageUsageFlagBits;
            }

            static INLINE VkImageAspectFlagBits GetVkImageAspect(TextureFormat format)
            {
                VkImageAspectFlagBits aspectMask;

                switch (format)
                {
                    case BX_FORMAT_R8:
                    case BX_FORMAT_RG8:
                    case BX_FORMAT_RGB8:
                    case BX_FORMAT_RGBA8:
                    case BX_FORMAT_RGBA:
                    case BX_FORMAT_RGBA16:
                    case BX_FORMAT_RGBA32:
                    case BX_FORMAT_RGBA64:
                    case BX_FORMAT_SRGB:
                        aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                        break;
                    case BX_FORMAT_DEPTH16:
                    case BX_FORMAT_DEPTH16_STENCIL:
                    case BX_FORMAT_DEPTH24_STENCIL:
                    case BX_FORMAT_DEPTH32:
                    case BX_FORMAT_DEPTH32_STENCIL:
                        aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                        break;
                    default:
                        assert(FALSE);
                        break;
                }

                return aspectMask;
            }

            static INLINE BOOL GetBxStatus(
                const VkResult vkResult)
            {
                return ((vkResult == VK_SUCCESS) ? BX_SUCCESS : BX_FAIL);
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
    }
}