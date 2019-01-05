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
#include "../Buffer/VulkanBuffer.h"

namespace VulkanEngine
{
    namespace Texture
    {
        VulkanTexture2D::VulkanTexture2D(
            const VkDevice* const    pDevice,
            Mgr::CmdBufferMgr* const pCmdBufferMgr,
            Texture2DCreateData*     pTex2DCreateData)
            : Texture2D(pTex2DCreateData),
              m_pDevice(pDevice),
              m_pCmdBufferMgr(pCmdBufferMgr)
        {
            m_texImage       = { *m_pDevice, vkDestroyImage };
            m_texImageMemory = { *m_pDevice, vkFreeMemory   };
        }

        VulkanTexture2D::~VulkanTexture2D()
        {
        }

        BOOL VulkanTexture2D::create(
            const VkPhysicalDevice hwDevice)
        {
            BOOL result = BX_SUCCESS;

            // The texture format must be optimized at this point
            assert(m_texOptimize == TRUE);

            // Create image handle
            VkImageCreateInfo imageCreateInfo = {};
            imageCreateInfo.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageCreateInfo.imageType         = VK_IMAGE_TYPE_2D;
            imageCreateInfo.extent.width      = m_textureWidth;
            imageCreateInfo.extent.height     = m_textureHeight;
            imageCreateInfo.extent.depth      = 1;
            imageCreateInfo.mipLevels         = m_mipmap;
            imageCreateInfo.arrayLayers       = 1;
            imageCreateInfo.format            = Utility::VulkanUtility::GetVkImageFormat(m_loadFormat);

            if (m_texOptimize == TRUE)
            {
                imageCreateInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
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

            imageCreateInfo.usage       =
                (VK_IMAGE_USAGE_TRANSFER_DST_BIT | Utility::VulkanUtility::GetVkImageUsage(m_usage));
            imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageCreateInfo.samples     = Utility::VulkanUtility::GetVkSampleCount(m_samples);
            imageCreateInfo.flags       = 0;

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

            // Create image buffer
            Buffer::VulkanBufferBase vkImageRawBuffer(m_pDevice, m_pCmdBufferMgr);

            Buffer::BxBufferCreateInfo imageRawBufferCreateInfo = {};
            imageRawBufferCreateInfo.bufferData                 = GetTextureData();
            imageRawBufferCreateInfo.bufferOptimization         = FALSE;
            imageRawBufferCreateInfo.bufferSize                 = m_textureWidth * m_textureHeight * 4;
            imageRawBufferCreateInfo.bufferUsage                = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

            vkImageRawBuffer.createBuffer(hwDevice, imageRawBufferCreateInfo);
            
            FreeTextureData();

            // Image layout transfer
            Buffer::BxLayoutTransitionInfo transitionInfo    = {};
            transitionInfo.oldLayout                         = VK_IMAGE_LAYOUT_UNDEFINED;
            transitionInfo.newLayout                         = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            transitionInfo.subResourceInfo[0].aspectMask     = Utility::VulkanUtility::GetVkImageAspect(m_storeFormat);
            transitionInfo.subResourceInfo[0].baseArrayLayer = 0;
            transitionInfo.subResourceInfo[0].layerNum       = 1;
            transitionInfo.subResourceInfo[0].baseMipLevel   = 0;
            transitionInfo.subResourceInfo[0].mipmapLevelNum = m_mipmap;

            m_pCmdBufferMgr->imageLayoutTransition(m_texImage, transitionInfo);

            // Copy the image data from buffer to image
            std::vector<Buffer::BxBufferToImageCopyInfo> bufferToImageCopyInfo(1);
            bufferToImageCopyInfo[0].bufferInfo.bufferOffset      = 0;
            bufferToImageCopyInfo[0].bufferInfo.bufferRowLength   = 0;
            bufferToImageCopyInfo[0].bufferInfo.bufferImageHeight = 0;
            bufferToImageCopyInfo[0].subResourceInfo              = transitionInfo.subResourceInfo[0];
            bufferToImageCopyInfo[0].imageInfo.imageOffset        = { 0, 0, 0 };
            bufferToImageCopyInfo[0].imageInfo.imageExtent        = { m_textureWidth, m_textureHeight, 1};

            m_pCmdBufferMgr->copyBufferToImage(vkImageRawBuffer.GetBuffer(), m_texImage, bufferToImageCopyInfo);

            // Transfer the image layout according to usage
            switch (m_usage)
            {
                case BX_TEXTURE_USAGE_SAMPLED:
                    transitionInfo.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                    transitionInfo.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    m_pCmdBufferMgr->imageLayoutTransition(m_texImage, transitionInfo);
                    break;
                case BX_TEXTURE_USAGE_RENDER_TARGET:
                    NotImplemented();
                    break;
                case BX_TEXTURE_USAGE_COLOR_ATTACHMENT:
                    NotImplemented();
                    break;
                case BX_TEXTURE_USAGE_DEPTH_STENCIL_ATTACHMENT:
                    NotImplemented();
                    break;
                default:
                    assert(FALSE);
                    break;
            }

            return result;
        }

        /*VulkanTexture2D VulkanTexture2D::CreateTexture2D(
            const std::string&       fileName,
            const VkPhysicalDevice   hwDevice,
            const VkDevice* const    pDevice,
            Texture2DCreateData*     pTex2DCreateData,
            Mgr::CmdBufferMgr* const pCmdBufferMgr)
        {
            Texture2DCreateData createData = {};

            int texChannels;

            createData.textureData = ::Texture::TextureBase::ReadImageData(
                          fileName,
                          reinterpret_cast<int*>(&createData.texWidth),
                          reinterpret_cast<int*>(&createData.texHeight),
                          &texChannels);

            return VulkanTexture2D(pDevice, pCmdBufferMgr, pTex2DCreateData);
        }*/
    }
}