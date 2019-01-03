//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "VulkanTexture2D.h"
#include "../Core/VulkanUtility.h"

namespace VulkanEngine
{
    namespace Texture
    {
        VulkanTexture2D::VulkanTexture2D(
            const VkDevice* const pDevice,
            Texture2DCreateData*  pTex2DCreateData)
            : Texture2D(pTex2DCreateData),
              m_pDevice(pDevice)
        {
            m_texImage       = { *m_pDevice, vkDestroyImage };
            m_texImageMemory = { *m_pDevice, vkFreeMemory };
        }

        VulkanTexture2D::~VulkanTexture2D()
        {
        }

        BOOL VulkanTexture2D::create(
            const VkPhysicalDevice hwDevice)
        {
            BOOL result = BX_SUCCESS;

            // Create image handle
            VkImageCreateInfo imageCreateInfo = {};
            imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
            imageCreateInfo.extent.width = m_textureWidth;
            imageCreateInfo.extent.height = m_textureHeight;
            imageCreateInfo.extent.depth = 1;
            imageCreateInfo.mipLevels = m_mipmap;
            imageCreateInfo.arrayLayers = 1;
            imageCreateInfo.format = Utility::VulkanUtility::GetVkImageFormat(m_loadFormat);

            if (m_texOptimize == TRUE)
            {
                imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
                imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            }
            else
            {
                imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;

                if (m_texPerserve == TRUE)
                {
                    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
                }
                else
                {
                    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                }
            }

            imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageCreateInfo.samples = Utility::VulkanUtility::GetVkSampleCount(m_samples);
            imageCreateInfo.flags = 0;

            VkResult createImageResult =
                vkCreateImage(*m_pDevice, &imageCreateInfo, NULL, m_texImage.replace());
            result = Utility::VulkanUtility::GetBxStatus(createImageResult);

            assert(result == BX_SUCCESS);

            // Allocate image memory
            VkMemoryRequirements imageMemoryRequirement;
            vkGetImageMemoryRequirements(*m_pDevice, m_texImage, &imageMemoryRequirement);

            VkPhysicalDeviceMemoryProperties hwMemoryProps;
            vkGetPhysicalDeviceMemoryProperties(hwDevice, &hwMemoryProps);

            VkMemoryAllocateInfo imageMemAllocInfo = {};
            imageMemAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            imageMemAllocInfo.allocationSize = imageMemoryRequirement.size;
            imageMemAllocInfo.memoryTypeIndex =
                Utility::VulkanUtility::FindMemoryType(
                    hwMemoryProps,
                    imageMemoryRequirement.memoryTypeBits,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            VkResult imageMemAllocResult =
                vkAllocateMemory(*m_pDevice, &imageMemAllocInfo, NULL, m_texImageMemory.replace());

            result = Utility::VulkanUtility::GetBxStatus(imageMemAllocResult);

            assert(result == BX_SUCCESS);

            // Bind image handle to memory
            vkBindImageMemory(*m_pDevice, m_texImage, m_texImageMemory, 0);

            return result;
        }

        /*VulkanTexture2D VulkanTexture2D::CreateTexture2D(
            const std::string&     fileName,
            const VkPhysicalDevice hwDevice)
        {
        }*/
    }
}