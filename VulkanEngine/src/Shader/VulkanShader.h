//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../Core/VulkanPCH.h"
#include "../Core/VulkanUtility.h"

namespace VulkanEngine
{
    namespace Shader
    {
        class VulkanShader
        {
        public:
            VulkanShader(
                const VkDevice& device);

            ~VulkanShader();

            static void PreprocessingGLSL(
                const std::string& glslFileName);

            static void CompileGLSL(
                const std::string& glslFileName,
                const BOOL         preprocess);

        protected:
            const VkDevice& m_device; // Reference to device
        };
    }
}