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

using namespace Texture;

namespace VulkanEngine
{
    namespace Texture
    {
        class VulkanTexture2D : public Texture2D
        {
        public:
            VulkanTexture2D(
                Texture2DCreateData* pTex2DCreateData);

            ~VulkanTexture2D();

        private:
            VkImage        m_texImage;
            VkDeviceMemory m_texImageMemory;
        };
    }
}