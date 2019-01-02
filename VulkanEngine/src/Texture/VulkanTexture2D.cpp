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
            Texture2DCreateData* pTex2DCreateData)
            : Texture2D(pTex2DCreateData)
        {
            VkImageCreateInfo imageCreateInfo = {};
            imageCreateInfo.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageCreateInfo.imageType         = VK_IMAGE_TYPE_2D;
            imageCreateInfo.extent.width      = m_textureWidth;
            imageCreateInfo.extent.height     = m_textureHeight;
            imageCreateInfo.extent.depth      = 1;
            imageCreateInfo.mipLevels         = m_mipmap;
            imageCreateInfo.arrayLayers       = 1;
            imageCreateInfo.format            = Utility::VulkanUtility::GetVkImageFormat(m_loadFormat);

        }

        VulkanTexture2D::~VulkanTexture2D()
        {
        }
    }
}