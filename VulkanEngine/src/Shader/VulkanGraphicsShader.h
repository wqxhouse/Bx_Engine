//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "VulkanShader.h"

namespace VulkanEngine
{
    namespace Shader
    {
        struct BxShaderInfo
        {
            std::string shaderFile;
            std::string shaderEntry = "main";
        };

        struct BxShaderMeta
        {
            BxShaderInfo vertexShaderInfo;
            BxShaderInfo tesShaderInfo;
            BxShaderInfo tcsShaderInfo;
            BxShaderInfo geometryShaderInfo;
            BxShaderInfo fragmentShaderInfo;
        };

        class VulkanGraphicsShader : public VulkanShader
        {
        public:
            VulkanGraphicsShader(
                const VkDevice& device);

            ~VulkanGraphicsShader();

            const std::vector<VkPipelineShaderStageCreateInfo>& createPipelineShaderStages(
                const BxShaderMeta& shaderMeta);

            inline const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStages() const
            {
                return m_shaderStages;
            }

        private:
            void createShaderModule(
                const std::string&   shaderFile,
                const BX_SHADER_TYPE shaderType);

            VkPipelineShaderStageCreateInfo createShaderStage(
                const VkShaderModule&       shaderModule,
                const VkShaderStageFlagBits stageBit,
                const std::string&          shaderEntry);

            struct BxShaderSource
            {
                BX_SHADER_TYPE           shaderType;
                std::vector<char>        source;
                VDeleter<VkShaderModule> shaderModule;
            };

            std::string                                  m_shaderPath;
            std::vector<BxShaderSource>                  m_shaderSources;
            std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;

            union ShaderFlags
            {
                UINT value;
                struct
                {
                    UINT vertexShaderBit : 1;
                    UINT tesShaderBit : 1;
                    UINT tcsShaderBit : 1;
                    UINT geometryShaderBit : 1;
                    UINT fragmentShaderBit : 1;
                    UINT reserve : 27;
                };
            } m_shaderFlags;
        };
    }
}