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
#include <Texture/Texture3D.h>
#include <Texture/Cubemap.h>

#include "../Core/VulkanPCH.h"
#include "../Context/CmdBufferMgr.h"

namespace VulkanEngine
{
    namespace Texture
    {
        // Texture Base
        class VulkanTextureBase
        {
        public:
            VulkanTextureBase(
                const VkDevice* const           pDevice,
                Mgr::CmdBufferMgr* const        pCmdBufferMgr)
                : m_pDevice(pDevice),
                  m_pCmdBufferMgr(pCmdBufferMgr)
            {
                m_texImageView   = { *m_pDevice, vkDestroyImageView };
                m_texSampler     = { *m_pDevice, vkDestroySampler   };

                m_textureFlags.value = 0;
            }

            virtual ~VulkanTextureBase()
            {
                clean();
            }

            virtual BOOL initialize() = 0;

            virtual BOOL create(
                const VkPhysicalDevice hwDevice) = 0;

            virtual BOOL createTextureImageView() = 0;

            virtual BOOL createSampler(
                const ::Texture::TextureSamplerCreateData* pSamplerCreateData,
                const BOOL                                 isSamplerAnisotropySupport) = 0;

            virtual BOOL createDescriptorSetLayout(
                const UINT               bindingPoint,
                const UINT               descriptorNum,
                const VkShaderStageFlags stageFlags) = 0;

            virtual void clean() {}

            INLINE VkImage     GetTextureImage()     const { return m_texImage;     }
            INLINE VkImageView GetTextureImageView() const { return m_texImageView; }
            INLINE VkSampler   GetTextureSampler()   const { return m_texSampler;   }

            virtual INLINE TextureType   GetTextureType()        const = 0;

            virtual INLINE UINT          GetTextureWidth()       const = 0;
            virtual INLINE UINT          GetTextureHeight()      const = 0;
            virtual INLINE UINT          GetTextureLayers()      const = 0;

            virtual INLINE UINT          GetSampleNumber()       const = 0;

            virtual INLINE TextureUsage  GetTextureUsage()       const = 0;
            virtual INLINE TextureFormat GetTextureFormat()      const = 0;

            virtual INLINE BOOL          IsGenMipmap()           const = 0;
            virtual INLINE BOOL          IsTextureOptimize()     const = 0;
            virtual INLINE BOOL          IsTextureDataPerserve() const = 0;

            virtual INLINE const void*   GetTextureData()        const = 0;
            virtual INLINE void          FreeTextureData()             = 0;

        protected:
            const VkDevice* const    m_pDevice;
            Mgr::CmdBufferMgr* const m_pCmdBufferMgr;

            VDeleter<VkImage>        m_texImage;
            VDeleter<VkDeviceMemory> m_texImageMemory;
            VDeleter<VkImageView>    m_texImageView;
            VDeleter<VkSampler>      m_texSampler;

            UINT                     m_mipmapLevel;

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

        // Texture 2D
        class VulkanTexture2D : public VulkanTextureBase
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
                const VDeleter<VkImage>         image);

            ~VulkanTexture2D();

            BOOL initialize();

            BOOL create(
                const VkPhysicalDevice hwDevice);

            BOOL createTextureImageView();

            BOOL createSampler(
                const ::Texture::TextureSamplerCreateData* pSamplerCreateData,
                const BOOL                                 isSamplerAnisotropySupport);

            BOOL createDescriptorSetLayout(
                const UINT               bindingPoint,
                const UINT               descriptorNum,
                const VkShaderStageFlags stageFlags);

            INLINE TextureType   GetTextureType()        const { return m_texture2D.GetTextureType();        }

            INLINE UINT          GetTextureWidth()       const { return m_texture2D.GetTextureWidth();       }
            INLINE UINT          GetTextureHeight()      const { return m_texture2D.GetTextureHeight();      }
            INLINE UINT          GetTextureLayers()      const { return 1;                                   }

            INLINE UINT          GetSampleNumber()       const { return m_texture2D.GetSampleNumber();       }

            INLINE TextureUsage  GetTextureUsage()       const { return m_texture2D.GetTextureUsage();       }
            INLINE TextureFormat GetTextureFormat()      const { return m_texture2D.GetTextureFormat();      }

            INLINE BOOL          IsGenMipmap()           const { return m_texture2D.IsGenMipmap();           }
            INLINE BOOL          IsTextureOptimize()     const { return m_texture2D.IsTextureOptimize();     }
            INLINE BOOL          IsTextureDataPerserve() const { return m_texture2D.IsTextureDataPerserve(); }

            INLINE const void*   GetTextureData()        const { return m_texture2D.GetTextureData();        }
            INLINE void          FreeTextureData()             { m_texture2D.FreeTextureData();              }

        private:
            ::Texture::Texture2D m_texture2D;
        };

        // Texture 3D
        class VulkanTexture3D : public VulkanTextureBase
        {
        public:
        private:
        };

        // Cubemap
        class VulkanTextureCubemap : public VulkanTextureBase
        {
        };
    }
}