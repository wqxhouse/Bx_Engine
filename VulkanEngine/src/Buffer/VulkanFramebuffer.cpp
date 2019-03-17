//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "VulkanFramebuffer.h"

#include "../Texture/VulkanTexture.h"

namespace VulkanEngine
{
    namespace Buffer
    {
        VulkanFramebuffer::VulkanFramebuffer(
            const VkDevice* const pDevice)
            : m_pDevice(pDevice)
        {
            m_framebuffer = { *m_pDevice, vkDestroyFramebuffer };
        }

        VulkanFramebuffer::~VulkanFramebuffer()
        {
        }

        BOOL VulkanFramebuffer::createFramebuffer(
            const FramebufferCreateData& attachmentsCreateData)
        {
            BOOL result = BX_SUCCESS;

            size_t attachmentNum = attachmentsCreateData.ppAttachments->size();

            std::vector<VkImageView> attachmentImageViews(attachmentNum);
            m_pTextureList.resize(attachmentNum);

            for (size_t i = 0; i < attachmentNum; ++i)
            {
                VulkanAttachment*           pAttachment = attachmentsCreateData.ppAttachments->at(i);
                Texture::VulkanTextureBase* pTex = pAttachment->pTex;
                m_pTextureList[i]                = pTex;

                if (pAttachment->sampleNum == 1)
                {
                    attachmentImageViews[i] = pTex->GetTextureImageView();
                }
                else
                {
                    attachmentImageViews[i] = pTex->GetTextureResolveImageView();
                }
            }

            VkFramebufferCreateInfo framebufferCreateInfo = {};
            framebufferCreateInfo.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.attachmentCount         = static_cast<UINT>(attachmentNum);
            framebufferCreateInfo.pAttachments            = attachmentImageViews.data();
            framebufferCreateInfo.renderPass              = attachmentsCreateData.renderPass;
            framebufferCreateInfo.width                   = attachmentsCreateData.width;
            framebufferCreateInfo.height                  = attachmentsCreateData.height;
            framebufferCreateInfo.layers                  = attachmentsCreateData.layers;

            VkResult framebufferCreateResult =
                vkCreateFramebuffer(*m_pDevice, &framebufferCreateInfo, NULL, m_framebuffer.replace());

            result = Utility::VulkanUtility::GetBxStatus(framebufferCreateResult);

            assert(result == BX_SUCCESS);

            return result;
        }
    }
}