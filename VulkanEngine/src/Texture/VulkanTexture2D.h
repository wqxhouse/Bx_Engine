//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "VulkanTextureBase.h"
#include "../Core/VulkanPCH.h"
#include "../Context/CmdBufferMgr.h"

namespace VulkanEngine
{
    namespace Texture
    {
        class VulkanTexture2D : public ::Texture::Texture2D,
                                public VulkanTextureBase
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

            BOOL createDescriptorSetLayout(
                const UINT               bindingPoint,
                const UINT               descriptorNum,
                const VkShaderStageFlags stageFlags);

            /*static VulkanTexture2D& CreateTexture2D(
                const std::string&       fileName,
                const VkPhysicalDevice   hwDevice,
                const VkDevice* const    pDevice,
                Texture2DCreateData*     pTex2DCreateData,
                Mgr::CmdBufferMgr* const pCmdBufferMgr);*/
        };
    }
}