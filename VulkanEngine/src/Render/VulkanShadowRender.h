//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2019
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include <Scene/RenderScene.h>

namespace VulkanEngine
{
    namespace Render
    {
        class VulkanShadowRender
        {
        public:
            VulkanShadowRender();
            ~VulkanShadowRender();

            BOOL initializeShadowRender();

        private:

        };
    }
}