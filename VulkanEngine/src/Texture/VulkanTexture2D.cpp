//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "VulkanTexture.h"
#include "../Core/VulkanUtility.h"
#include "../Buffer/VulkanBuffer.h"

namespace VulkanEngine
{
    namespace Texture
    {
        VulkanTexture2D::VulkanTexture2D(
            const VkDevice* const           pDevice,
            Mgr::CmdBufferMgr* const        pCmdBufferMgr,
            ::Texture::Texture2DCreateData* pTex2DCreateData,
            const BOOL                      isResolve)
            : m_texture2D(pTex2DCreateData),
              VulkanTextureBase(pDevice, pCmdBufferMgr, FALSE, isResolve)
        {
            m_texImage       = { *m_pDevice, vkDestroyImage };
            m_texImageMemory = { *m_pDevice, vkFreeMemory   };
        }

        VulkanTexture2D::VulkanTexture2D(
            const VkDevice* const           pDevice,
            Mgr::CmdBufferMgr* const        pCmdBufferMgr,
            ::Texture::Texture2DCreateData* pTex2DCreateData,
            const VDeleter<VkImage>         image,
            const BOOL                      isResolve)
            : m_texture2D(pTex2DCreateData),
              VulkanTextureBase(pDevice, pCmdBufferMgr, TRUE, isResolve)
        {
            m_texImage = image;
        }

        VulkanTexture2D::~VulkanTexture2D()
        {
        }

        BOOL VulkanTexture2D::initialize()
        {
            BOOL result = BX_SUCCESS;

            UINT texWidth  = GetTextureWidth();
            UINT texHeight = GetTextureHeight();

            m_mipmapLevel = 1;

            if (IsGenMipmap() == TRUE)
            {
                m_mipmapLevel = static_cast<UINT>(std::floor(std::log2(std::max(texWidth, texHeight))) + 1);
            }

            return result;
        }

        BOOL VulkanTexture2D::create(
            const VkPhysicalDevice hwDevice)
        {
            BOOL result = BX_SUCCESS;

            const UINT sampleNum = GetSampleNumber();

            if (sampleNum == 1)
            {
                if (m_textureFlags.isExternal == FALSE)
                {
                    result = createImage2D(hwDevice, m_texImage.replace(), m_texImageMemory.replace(), 1);

                    assert(result == BX_SUCCESS);
                }

                result = createTextureImageView(m_texImage, m_texImageView.replace());
            }
            else
            {
                if (IsResolve() == TRUE)
                {
                    if (m_textureFlags.isExternal == FALSE)
                    {
                        result = createImage2D(hwDevice, m_texImage.replace(), m_texImageMemory.replace(), 1);

                        assert(result == BX_SUCCESS);
                    }

                    result = createTextureImageView(m_texImage, m_texImageView.replace());

                    assert(result = BX_SUCCESS);
                }

                result = createImage2D(hwDevice, m_texMsaaImage.replace(), m_texMsaaImageMemory.replace(), sampleNum);

                assert(result == BX_SUCCESS);

                result = createTextureImageView(m_texMsaaImage, m_texMsaaImageView.replace());

                assert(result == BX_SUCCESS);
            }

            return result;
        }

        BOOL VulkanTexture2D::createSampler(
            const ::Texture::TextureSamplerCreateData* pSamplerCreateData,
            const BOOL                                 isSamplerAnisotropySupport)
        {
            BOOL result = BX_SUCCESS;

            VkSamplerCreateInfo samplerCreateInfo     = {};
            samplerCreateInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerCreateInfo.magFilter               = Utility::VulkanUtility::GetVkFilter(pSamplerCreateData->magFilter);
            samplerCreateInfo.minFilter               = Utility::VulkanUtility::GetVkFilter(pSamplerCreateData->minFilter);
            samplerCreateInfo.addressModeU            = Utility::VulkanUtility::GetVkAddressMode(pSamplerCreateData->addressingModeU);
            samplerCreateInfo.addressModeV            = Utility::VulkanUtility::GetVkAddressMode(pSamplerCreateData->addressingModeV);
            samplerCreateInfo.borderColor             = Utility::VulkanUtility::GetVkBorderColor(pSamplerCreateData->borderColor);
            samplerCreateInfo.unnormalizedCoordinates = ((pSamplerCreateData->normalize == TRUE) ? VK_FALSE : VK_TRUE);
            samplerCreateInfo.compareEnable           = VK_FALSE;
            samplerCreateInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
            samplerCreateInfo.mipmapMode              = Utility::VulkanUtility::GetVkMipmapMode(pSamplerCreateData->mipmapFilter);
            samplerCreateInfo.mipLodBias              = 0.0f;
            samplerCreateInfo.minLod                  = 0.0f;
            samplerCreateInfo.maxLod                  = ((IsGenMipmap() == TRUE) ? static_cast<float>(m_mipmapLevel) : 0.0f);

            if (isSamplerAnisotropySupport == TRUE)
            {
                samplerCreateInfo.anisotropyEnable = VK_TRUE;
                samplerCreateInfo.maxAnisotropy    = pSamplerCreateData->anisotropyNum;
            }
            else
            {
                samplerCreateInfo.anisotropyEnable = VK_FALSE;
                samplerCreateInfo.maxAnisotropy    = 1;
            }

            VkResult createSamplerResult = vkCreateSampler(*m_pDevice, &samplerCreateInfo, NULL, m_texSampler.replace());
            result = Utility::VulkanUtility::GetBxStatus(createSamplerResult);

            assert(result == BX_SUCCESS);

            return result;
        }

        BOOL VulkanTexture2D::createImage2D(
            const VkPhysicalDevice hwDevice,
            VkImage*               pImage,
            VkDeviceMemory*        pImageMemory,
            const UINT             sampleCount)
        {
            BOOL result = BX_SUCCESS;

            const UINT texWidth  = GetTextureWidth();
            const UINT texHeight = GetTextureHeight();

            const TextureUsage usage = GetTextureUsage();

            // The texture format must be optimized at this point
            assert(IsTextureOptimize() == TRUE);

            // Create image handle
            VkImageCreateInfo imageCreateInfo = {};
            imageCreateInfo.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageCreateInfo.imageType         = VK_IMAGE_TYPE_2D;
            imageCreateInfo.extent.width      = texWidth;
            imageCreateInfo.extent.height     = texHeight;
            imageCreateInfo.extent.depth      = 1;
            imageCreateInfo.mipLevels         = ((sampleCount == 1) ? m_mipmapLevel : 1);
            imageCreateInfo.arrayLayers       = 1;
            imageCreateInfo.format            = Utility::VulkanUtility::GetVkImageFormat(GetTextureFormat());

            if (IsTextureOptimize() == TRUE)
            {
                imageCreateInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
                imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            }
            else
            {
                imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;

                if (IsTextureDataPerserve() == TRUE)
                {
                    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
                }
                else
                {
                    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                }
            }

            imageCreateInfo.usage       =
                (VK_IMAGE_USAGE_TRANSFER_DST_BIT | Utility::VulkanUtility::GetVkImageUsage(usage));
            imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageCreateInfo.samples     = Utility::VulkanUtility::GetVkSampleCount(sampleCount);
            imageCreateInfo.flags       = 0;

            // The image needs to be used as blit source
            if (IsGenMipmap() == TRUE)
            {
                imageCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
            }

            VkResult createImageResult =
                vkCreateImage(*m_pDevice, &imageCreateInfo, NULL, pImage);
            result = Utility::VulkanUtility::GetBxStatus(createImageResult);

            assert(result == BX_SUCCESS);

            // Allocate image memory
            VkMemoryRequirements imageMemoryRequirement;
            vkGetImageMemoryRequirements(*m_pDevice, *pImage, &imageMemoryRequirement);

            VkPhysicalDeviceMemoryProperties hwMemoryProps;
            vkGetPhysicalDeviceMemoryProperties(hwDevice, &hwMemoryProps);

            VkMemoryAllocateInfo imageMemAllocInfo = {};
            imageMemAllocInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            imageMemAllocInfo.allocationSize       = imageMemoryRequirement.size;
            imageMemAllocInfo.memoryTypeIndex      =
                Utility::VulkanUtility::FindMemoryType(
                    hwMemoryProps,
                    imageMemoryRequirement.memoryTypeBits,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            VkResult imageMemAllocResult =
                vkAllocateMemory(*m_pDevice, &imageMemAllocInfo, NULL, pImageMemory);

            result = Utility::VulkanUtility::GetBxStatus(imageMemAllocResult);

            assert(result == BX_SUCCESS);

            // Bind image handle to memory
            vkBindImageMemory(*m_pDevice, *pImage, *pImageMemory, 0);            

            Buffer::BxLayoutTransitionInfo transitionInfo = {};
            if (((usage & BX_TEXTURE_USAGE_SAMPLED)                   != 0) &&
                (((usage & BX_TEXTURE_USAGE_RENDER_TARGET)            == 0) &&
                 ((usage & BX_TEXTURE_USAGE_COLOR_ATTACHMENT)         == 0) &&
                 ((usage & BX_TEXTURE_USAGE_DEPTH_STENCIL_ATTACHMENT) == 0)))
            {
                // Create image buffer
                Buffer::VulkanBufferBase vkImageRawBuffer(m_pDevice, m_pCmdBufferMgr);

                Buffer::BxBufferCreateInfo imageRawBufferCreateInfo = {};
                imageRawBufferCreateInfo.bufferData                 = ((sampleCount > 1) ? NULL : GetTextureData());
                imageRawBufferCreateInfo.bufferOptimization         = FALSE;
                imageRawBufferCreateInfo.bufferSize                 = texWidth * texHeight * 4;
                imageRawBufferCreateInfo.bufferUsage                = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                imageRawBufferCreateInfo.bufferDynamic              = FALSE;

                vkImageRawBuffer.createBuffer(hwDevice, imageRawBufferCreateInfo);

                if (sampleCount == 1)
                {
                    FreeTextureData();
                }

                // Image layout transfer
                transitionInfo.oldLayout                         = VK_IMAGE_LAYOUT_UNDEFINED;
                transitionInfo.newLayout                         = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                transitionInfo.subResourceInfo.resize(1);
                transitionInfo.subResourceInfo[0].aspectMask     =
                    Utility::VulkanUtility::GetVkImageAspect(GetTextureFormat());
                transitionInfo.subResourceInfo[0].baseArrayLayer = 0;
                transitionInfo.subResourceInfo[0].layerNum       = 1;
                transitionInfo.subResourceInfo[0].baseMipLevel   = 0;
                transitionInfo.subResourceInfo[0].mipmapLevelNum = m_mipmapLevel;

                result = m_pCmdBufferMgr->imageLayoutTransition(*pImage, transitionInfo);

                assert(result == BX_SUCCESS);

                // Copy the image data from buffer to image
                std::vector<Buffer::BxBufferToImageCopyInfo> bufferToImageCopyInfo(1);
                bufferToImageCopyInfo[0].bufferInfo.bufferOffset      = 0;
                bufferToImageCopyInfo[0].bufferInfo.bufferRowLength   = 0;
                bufferToImageCopyInfo[0].bufferInfo.bufferImageHeight = 0;
                bufferToImageCopyInfo[0].subResourceInfo              = transitionInfo.subResourceInfo[0];
                bufferToImageCopyInfo[0].imageInfo.imageOffset        = { 0, 0, 0 };
                bufferToImageCopyInfo[0].imageInfo.imageExtent        = { texWidth, texHeight, 1};

                result = m_pCmdBufferMgr->copyBufferToImage(vkImageRawBuffer.GetBuffer(), *pImage, bufferToImageCopyInfo);

                assert(result == BX_SUCCESS);
            }

            // Transfer the image layout according to usage
            if (((usage & BX_TEXTURE_USAGE_SAMPLED)                 != 0) ||
                ((usage & BX_TEXTURE_USAGE_VULKAN_INPUT_ATTACHMENT) != 0))
            {
                // TODO: Re-organize if-else blocks here
                if ((IsGenMipmap()                                        == FALSE) &&
                    (((usage & BX_TEXTURE_USAGE_RENDER_TARGET)            == 0)     &&
                     ((usage & BX_TEXTURE_USAGE_COLOR_ATTACHMENT)         == 0)     &&
                     ((usage & BX_TEXTURE_USAGE_DEPTH_STENCIL_ATTACHMENT) == 0)))
                {
                    transitionInfo.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                    transitionInfo.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                }
                else
                {
                    if (IsGenMipmap() == TRUE)
                    {
                        transitionInfo.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                        transitionInfo.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                    }
                    else if ((usage & BX_TEXTURE_USAGE_COLOR_ATTACHMENT) != 0)
                    {
                        transitionInfo.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                        transitionInfo.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    }
                    else if ((usage & BX_TEXTURE_USAGE_DEPTH_STENCIL_ATTACHMENT) != 0)
                    {
                        transitionInfo.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                        transitionInfo.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                    }
                    else
                    {
                        NotSupported();
                    }
                }
            }
            else if ((usage & BX_TEXTURE_USAGE_COLOR_ATTACHMENT) != 0)
            {
                transitionInfo.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                transitionInfo.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            }
            else if ((usage& BX_TEXTURE_USAGE_DEPTH_STENCIL_ATTACHMENT) != 0)
            {
                transitionInfo.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                transitionInfo.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            }
            else
            {
                NotSupported();
            }

            result = m_pCmdBufferMgr->imageLayoutTransition(*pImage, transitionInfo);

            assert(result == BX_SUCCESS);

            if (sampleCount == 1 && IsGenMipmap() == TRUE)
            {
                m_pCmdBufferMgr->genMipmaps(*pImage, GetTextureWidth(), GetTextureHeight(), m_mipmapLevel, 1);

                // Transfer the image layout to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL for preparing to be read by shader
                Buffer::BxLayoutTransitionInfo transitionInfo = {};
                transitionInfo.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                transitionInfo.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                transitionInfo.subResourceInfo.push_back({ VK_IMAGE_ASPECT_COLOR_BIT, 0, m_mipmapLevel, 0, 1 });

                m_pCmdBufferMgr->imageLayoutTransition(*pImage, transitionInfo);
            }

            return result;
        }

        BOOL VulkanTexture2D::createTextureImageView(
            const VkImage image,
            VkImageView*  pImageView)
        {
            BOOL result = BX_SUCCESS;

            VkImageViewCreateInfo imageViewCreateInfo           = {};
            imageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewCreateInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
            imageViewCreateInfo.image                           = image;
            imageViewCreateInfo.format                          =
                Utility::VulkanUtility::GetVkImageFormat(GetTextureFormat());
            imageViewCreateInfo.components.r                    = VK_COMPONENT_SWIZZLE_R;
            imageViewCreateInfo.components.g                    = VK_COMPONENT_SWIZZLE_G;
            imageViewCreateInfo.components.b                    = VK_COMPONENT_SWIZZLE_B;
            imageViewCreateInfo.components.a                    = VK_COMPONENT_SWIZZLE_A;

            imageViewCreateInfo.subresourceRange.aspectMask     =
                Utility::VulkanUtility::GetVkImageAspect(GetTextureFormat());
            imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            imageViewCreateInfo.subresourceRange.layerCount     = 1;
            imageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
            imageViewCreateInfo.subresourceRange.levelCount     = m_mipmapLevel;

            VkResult vkResult = vkCreateImageView(
                *m_pDevice, &imageViewCreateInfo, NULL, pImageView);
            result = Utility::VulkanUtility::GetBxStatus(vkResult);

            assert(result == BX_SUCCESS);

            return result;
        }
    }
}