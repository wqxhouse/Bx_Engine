//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
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
        class VulkanTexture2D : public ::Texture::Texture2D
        {
        public:
            VulkanTexture2D(
                const VkDevice* const           pDevice,
                Mgr::CmdBufferMgr* const        pCmdBufferMgr,
                ::Texture::Texture2DCreateData* pTex2DCreateData);

            // Bind the image which externally created (e.g. swapchain images)
            VulkanTexture2D(
                const VkDevice* const           pDevice,
                Mgr::CmdBufferMgr* const        pCmdBufferMgr,
                ::Texture::Texture2DCreateData* pTex2DCreateData,
                const VkImage                   image);

            ~VulkanTexture2D();

            BOOL create(
                const VkPhysicalDevice hwDevice);

            BOOL createTextureImageView();

            BOOL createSampler(
                const ::Texture::TextureSamplerCreateData& samplerCreateData,
                const BOOL                                 isSamplerAnisotropySupport);

            /*static VulkanTexture2D& CreateTexture2D(
                const std::string&       fileName,
                const VkPhysicalDevice   hwDevice,
                const VkDevice* const    pDevice,
                Texture2DCreateData*     pTex2DCreateData,
                Mgr::CmdBufferMgr* const pCmdBufferMgr);*/

            INLINE const VkImage&     GetTextureImage()     const { return m_texImage;     }
            INLINE const VkImageView& GetTextureImageView() const { return m_texImageView; }
            INLINE const VkSampler&   GetTextureSampler()   const { return m_texSampler;   }

        private:
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
                    UINT reserve    : 31;
                };
            } m_textureFlags;
        };
    }
}