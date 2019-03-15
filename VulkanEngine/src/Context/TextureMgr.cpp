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
            for (Texture::VulkanTextureBase* pTex : m_pTextureList)
            {
                if (pTex != NULL)
                {
                    switch (pTex->GetTextureType())
                    {
                        case TEXTURE_2D:
                            static_cast<Texture::VulkanTexture2D*>(pTex)->~VulkanTexture2D();
                            break;
                        case TEXTURE_3D:
                            NotImplemented();
                            break;
                        case TEXTURE_CUBEMAP:
                            NotImplemented();
                            break;
                        default:
                            break;
                    }
                }
            }
        }

        Texture::VulkanTexture2D* TextureMgr::createTexture2D(
            ::Texture::Texture2DCreateData* pTexture2DCreateData,
            const BOOL                      isResolve)
        {
            BOOL status = BX_SUCCESS;

            assert(pTexture2DCreateData->texUsage          == BX_TEXTURE_USAGE_SAMPLED &&
                   pTexture2DCreateData->pSamplerCreateData != NULL);

            Texture::VulkanTexture2D* pNewTexture2D =
                BX_NEW(Texture::VulkanTexture2D, m_textureMgrArena)(m_pDevice, m_pCmdMgr, pTexture2DCreateData, isResolve);

            status = pNewTexture2D->initialize();
            status = pNewTexture2D->create(*m_pHwDevice);

            if (pTexture2DCreateData->texUsage == BX_TEXTURE_USAGE_SAMPLED)
            {
                pNewTexture2D->createSampler(pTexture2DCreateData->pSamplerCreateData, m_isSamplerAnisotropySupport);
            }

            m_pTextureList.push_back(pNewTexture2D);

            return pNewTexture2D;
        }

        Texture::VulkanTexture2D* TextureMgr::createTexture2DRenderTargetFromImage(
            const UINT              texWidth,
            const UINT              texHeight,
            const UINT              samples,
            const TextureFormat     texFormat,
            BX_VULKAN_TEXTURE_USAGE vulkanUsage,
            const VDeleter<VkImage> image,
            const BOOL              isResolve)
        {
            BOOL status = BX_SUCCESS;

            TextureUsage usage = BX_TEXTURE_USAGE_RENDER_TARGET |
                                 vulkanUsage                    |
                                 Utility::VulkanUtility::GetTextureUsage(texFormat);

            ::Texture::Texture2DCreateData texture2DCreateData = {};
            texture2DCreateData.texUsage           = usage;
            texture2DCreateData.texWidth           = texWidth;
            texture2DCreateData.texHeight          = texHeight;
            texture2DCreateData.samples            = samples;
            texture2DCreateData.texLoadFormat      = texFormat;
            texture2DCreateData.texStoreFormat     = texFormat;
            texture2DCreateData.mipmap             = FALSE;
            texture2DCreateData.texOptimize        = TRUE;
            texture2DCreateData.texPerserve        = FALSE;
            texture2DCreateData.textureData        = NULL;
            texture2DCreateData.pSamplerCreateData = NULL;

            Texture::VulkanTexture2D* pNewTexture2D =
                BX_NEW(Texture::VulkanTexture2D, m_textureMgrArena)(m_pDevice, m_pCmdMgr, &texture2DCreateData, image, isResolve);

            status = pNewTexture2D->initialize();

            assert(status == BX_SUCCESS);

            status = pNewTexture2D->create(*m_pHwDevice);

            assert(status == BX_SUCCESS);

            m_pTextureList.push_back(pNewTexture2D);

            return pNewTexture2D;
        }

        Texture::VulkanTexture2D* TextureMgr::createTexture2DRenderTarget(
            const UINT              texWidth,
            const UINT              texHeight,
            const UINT              samples,
            const TextureFormat     texFormat,
            BX_VULKAN_TEXTURE_USAGE vulkanUsage,
            const BOOL              isResolve)
        {
            BOOL status = BX_SUCCESS;

            TextureUsage usage = vulkanUsage |
                                 Utility::VulkanUtility::GetTextureUsage(texFormat);

            ::Texture::Texture2DCreateData texture2DCreateData = {};
            texture2DCreateData.texUsage           = usage;
            texture2DCreateData.texWidth           = texWidth;
            texture2DCreateData.texHeight          = texHeight;
            texture2DCreateData.samples            = samples;
            texture2DCreateData.texLoadFormat      = texFormat;
            texture2DCreateData.texStoreFormat     = texFormat;
            texture2DCreateData.mipmap             = FALSE;
            texture2DCreateData.texOptimize        = TRUE;
            texture2DCreateData.texPerserve        = FALSE;
            texture2DCreateData.textureData        = NULL;
            texture2DCreateData.pSamplerCreateData = NULL;

            Texture::VulkanTexture2D* pNewTexture2D =
                BX_NEW(Texture::VulkanTexture2D, m_textureMgrArena)(m_pDevice, m_pCmdMgr, &texture2DCreateData, isResolve);

            status = pNewTexture2D->initialize();

            assert(status == BX_SUCCESS);

            status = pNewTexture2D->create(*m_pHwDevice);

            assert(status == BX_SUCCESS);

            m_pTextureList.push_back(pNewTexture2D);

            return pNewTexture2D;
        }

        Texture::VulkanTexture2D* TextureMgr::createTexture2DSampler(
            const std::string&                   imageFile,
            const UINT                           samples,
            const BOOL                           mipmap,
            const TextureFormat                  texLoadFormat,
            const TextureFormat                  texStoreFormat,
            ::Texture::TextureSamplerCreateData& textureSamplerCreateData)
        {
            BOOL status = BX_SUCCESS;

            int texWidth;
            int texHeight;
            int texChannels;

            ::Texture::Texture2DCreateData texture2DCreateData = {};

            texture2DCreateData.textureData =
                ::Texture::Texture2D::ReadImageData(imageFile, &texWidth, &texHeight, &texChannels);

            texture2DCreateData.texUsage           = BX_TEXTURE_USAGE_SAMPLED;
            texture2DCreateData.texWidth           = static_cast<UINT>(texWidth);
            texture2DCreateData.texHeight          = static_cast<UINT>(texHeight);
            texture2DCreateData.samples            = samples;
            texture2DCreateData.texLoadFormat      = texLoadFormat;
            texture2DCreateData.texStoreFormat     = texStoreFormat;
            texture2DCreateData.mipmap             = mipmap;
            texture2DCreateData.texOptimize        = TRUE;
            texture2DCreateData.texPerserve        = FALSE;
            texture2DCreateData.pSamplerCreateData = &textureSamplerCreateData;

            Texture::VulkanTexture2D* pNewTexture2D =
                BX_NEW(Texture::VulkanTexture2D, m_textureMgrArena)(m_pDevice, m_pCmdMgr, &texture2DCreateData, FALSE);

            status = pNewTexture2D->initialize();

            assert(status == BX_SUCCESS);

            status = pNewTexture2D->create(*m_pHwDevice);

            assert(status == BX_SUCCESS);

            status = pNewTexture2D->createSampler(texture2DCreateData.pSamplerCreateData, m_isSamplerAnisotropySupport);

            assert(status == BX_SUCCESS);

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