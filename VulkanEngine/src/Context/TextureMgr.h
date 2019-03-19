//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include <Memory/BxMemory.h>

#include "../Texture/VulkanTexture.h"

namespace VulkanEngine
{
    namespace Mgr
    {
        class TextureMgr
        {
        public:
            TextureMgr(
                const VkPhysicalDevice* pHwDevice,
                const VkDevice*         pDevice,
                CmdBufferMgr* const     pCmdMgr,
                const UINT              maxTextureNum,
                const BOOL              isSamplerAnisotropySupport);

            ~TextureMgr();

            Texture::VulkanTexture2D* createTexture2D(
                ::Texture::Texture2DCreateData* pTexture2DCreateData,
                const BOOL                      isResolve);

            Texture::VulkanTexture2D* createTexture2DRenderTargetFromImage(
                const UINT              texWidth,
                const UINT              texHeight,
                const UINT              samples,
                const TextureFormat     texFormat,
                BX_VULKAN_TEXTURE_USAGE vulkanUsage,
                const VDeleter<VkImage> image,
                const BOOL              isResolve);

            Texture::VulkanTexture2D* createTexture2DRenderTarget(
                const UINT              texWidth,
                const UINT              texHeight,
                const UINT              samples,
                const TextureFormat     texFormat,
                BX_VULKAN_TEXTURE_USAGE vulkanUsage,
                const BOOL              isResolve);

            Texture::VulkanTexture2D* createTexture2DSampler(
                const std::string&                   imageFile,
                const UINT                           samples,
                const BOOL                           mipmap,
                const TextureFormat                  texLoadFormat,
                const TextureFormat                  texStoreFormat,
                ::Texture::TextureSamplerCreateData& textureSamplerCreateData);

            Texture::VulkanTexture2D* createTexture2DSampler(
                const UINT                           texWidth,
                const UINT                           texHeight,
                const UINT                           samples,
                const BOOL                           mipmap,
                const TextureFormat                  texLoadFormat,
                const TextureFormat                  texStoreFormat,
                ::Texture::TextureSamplerCreateData& textureSamplerCreateData,
                const image_data*                    pImageData);

            Texture::VulkanTexture3D* createTexture3D(
                ::Texture::Texture3DCreateData* pTexture3DCreateData);

            Texture::VulkanTextureCubemap* createTextureCubemap(
                ::Texture::CubemapCreateData* pCubemapCreateData);

            Texture::VulkanTextureBase* GetTexture(const UINT texIndex) const
            {
                return m_pTextureList[texIndex];
            }

            INLINE const BOOL IsAnisotropySupport() const { return m_isSamplerAnisotropySupport; }

            static INLINE UINT GetTextureClassMaxSize()
            {
                UINT maxSize = 0;

                maxSize = static_cast<UINT>(std::max(sizeof(Texture::VulkanTexture2D), sizeof(Texture::VulkanTexture3D)));
                maxSize = std::max(maxSize, static_cast<UINT>(sizeof(Texture::VulkanTextureCubemap)));

                return maxSize;
            }

        private:
            const VkPhysicalDevice* m_pHwDevice;
            const VkDevice*         m_pDevice;
            CmdBufferMgr* const     m_pCmdMgr;

            Memory::MemoryLinear<DEFAULT_MAX_TEXTURE_NUM * DEFAULT_TEXTURE_CLASS_SIZE> m_textureMemory;
            Memory::LinearAllocator                                                    m_textureMgrAllocator;
            Memory::LinearMemoryArena                                                  m_textureMgrArena;

            const UINT m_maxTextureNum;

            std::vector<Texture::VulkanTextureBase*> m_pTextureList;

            // Sampler anisotropy support
            BOOL m_isSamplerAnisotropySupport;
        };
    }
}