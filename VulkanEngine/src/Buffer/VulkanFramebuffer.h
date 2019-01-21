//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include <Texture/TextureBase.h>

#include "../Core/VulkanPCH.h"
#include "../Texture/VulkanTexture.h"

namespace VulkanEngine
{
    namespace Buffer
    {
        struct FramebufferCreateData
        {
            std::vector<Texture::VulkanTextureBase*>* ppAttachments;
            VkRenderPass                              renderPass;
            UINT                                      width;
            UINT                                      height;
            UINT                                      layers;
        };

        class VulkanFramebuffer
        {
        public:
            VulkanFramebuffer(
                const VkDevice* const pDevice);

            ~VulkanFramebuffer();

            BOOL createFramebuffer(
                const FramebufferCreateData& attachmentsCreateData);

            const Texture::VulkanTextureBase* GetTexture(
                const UINT textureIndex) const
            {
                return m_pTextureList[textureIndex];
            }

            const VkFramebuffer GetFramebufferHandle() const { return m_framebuffer; }

        private:
            const VkDevice* const                    m_pDevice;

            VDeleter<VkFramebuffer>                  m_framebuffer;

            std::vector<Texture::VulkanTextureBase*> m_pTextureList;
        };
    }
}