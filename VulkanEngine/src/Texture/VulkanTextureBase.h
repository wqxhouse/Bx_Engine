//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include <Texture/Texture2D.h>

#include "../Core/VulkanPCH.h"
#include "../Context/CmdBufferMgr.h"

namespace VulkanEngine
{
    namespace Texture
    {
        class VulkanTextureBase
        {
        public:
            VulkanTextureBase(
                const VkDevice* const           pDevice,
                Mgr::CmdBufferMgr* const        pCmdBufferMgr)
                : m_pDevice(pDevice),
                  m_pCmdBufferMgr(pCmdBufferMgr)
            {
                m_texImage       = { *m_pDevice, vkDestroyImage     };
                m_texImageMemory = { *m_pDevice, vkFreeMemory       };
                m_texImageView   = { *m_pDevice, vkDestroyImageView };

                m_textureFlags.value = 0;
            }
            
            ~VulkanTextureBase() {}

            virtual BOOL create(
                const VkPhysicalDevice hwDevice) = 0;

            virtual BOOL createTextureImageView() = 0;

            virtual BOOL createSampler(
                const ::Texture::TextureSamplerCreateData& samplerCreateData,
                const BOOL                                 isSamplerAnisotropySupport) = 0;

            INLINE VkImage     GetTextureImage()     const { return m_texImage;     }
            INLINE VkImageView GetTextureImageView() const { return m_texImageView; }
            INLINE VkSampler   GetTextureSampler()   const { return m_texSampler;   }

        protected:
            const VkDevice* const    m_pDevice;
            Mgr::CmdBufferMgr* const m_pCmdBufferMgr;

            VDeleter<VkImage>        m_texImage;
            VDeleter<VkDeviceMemory> m_texImageMemory;
            VDeleter<VkImageView>    m_texImageView;
            VDeleter<VkSampler>      m_texSampler;

            union
            {
                UINT value;

                struct
                {
                    BOOL isExternal : 1;
                    UINT reserve : 31;
                };
            } m_textureFlags;
        };
    }
}