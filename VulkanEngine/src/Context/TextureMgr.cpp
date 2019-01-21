//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "TextureMgr.h"

namespace VulkanEngine
{
    namespace Mgr
    {
        TextureMgr::TextureMgr(
            const VkPhysicalDevice* pHwDevice,
            const VkDevice*         pDevice,
            CmdBufferMgr* const     pCmdMgr,
            const UINT              maxTextureNum,
            const BOOL              isSamplerAnisotropySupport)
            : m_pHwDevice(pHwDevice),
              m_pDevice(pDevice),
              m_pCmdMgr(pCmdMgr),
              m_textureMgrAllocator(&m_textureMemory),
              m_textureMgrArena(&m_textureMgrAllocator),
              m_maxTextureNum(maxTextureNum),
              m_pTextureList(m_maxTextureNum),
              m_isSamplerAnisotropySupport(isSamplerAnisotropySupport)
        {
            assert(GetTextureClassMaxSize() <= DEFAULT_TEXTURE_CLASS_SIZE);
        }

        TextureMgr::~TextureMgr()
        {
        }

        Texture::VulkanTexture2D* TextureMgr::createTexture2D(
            ::Texture::Texture2DCreateData* pTexture2DCreateData)
        {
            assert(pTexture2DCreateData->texUsage == BX_TEXTURE_USAGE_SAMPLED &&
                   pTexture2DCreateData->pSampleCreateData != NULL);

            Texture::VulkanTexture2D* pNewTexture2D =
                BX_NEW(Texture::VulkanTexture2D, m_textureMgrArena)(m_pDevice, m_pCmdMgr, pTexture2DCreateData);

            pNewTexture2D->create(*m_pHwDevice);
            pNewTexture2D->createTextureImageView();

            if (pTexture2DCreateData->texUsage == BX_TEXTURE_USAGE_SAMPLED)
            {
                pNewTexture2D->createSampler(pTexture2DCreateData->pSampleCreateData, m_isSamplerAnisotropySupport);
            }

            m_pTextureList.push_back(pNewTexture2D);

            return pNewTexture2D;
        }

        Texture::VulkanTexture3D* TextureMgr::createTexture3D(
            ::Texture::Texture3DCreateData* texture3DCreateData)
        {
            NotImplemented();
            return NULL;
        }

        Texture::VulkanTextureCubemap* TextureMgr::createTextureCubemap(
            ::Texture::CubemapCreateData* cubemapCreateData)
        {
            NotImplemented();
            return NULL;
        }


    }
}