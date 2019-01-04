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

using namespace Texture;

namespace VulkanEngine
{
    namespace Texture
    {
        class VulkanTexture2D : public Texture2D
        {
        public:
            VulkanTexture2D(
                const VkDevice* const    pDevice,
                Mgr::CmdBufferMgr* const pCmdBufferMgr,
                Texture2DCreateData*     pTex2DCreateData);

            ~VulkanTexture2D();

            BOOL create(
                const VkPhysicalDevice hwDevice);

            static VulkanTexture2D& CreateTexture2D(
                const std::string&       fileName,
                const VkPhysicalDevice   hwDevice,
                const VkDevice* const    pDevice,
                Texture2DCreateData*     pTex2DCreateData,
                Mgr::CmdBufferMgr* const pCmdBufferMgr);

        private:
            const VkDevice* const    m_pDevice;
            Mgr::CmdBufferMgr* const m_pCmdBufferMgr;

            VDeleter<VkImage>        m_texImage;
            VDeleter<VkDeviceMemory> m_texImageMemory;
        };
    }
}