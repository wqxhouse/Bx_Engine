//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "VulkanGraphicsShader.h"

namespace VulkanEngine
{
    namespace Shader
    {
        VulkanGraphicsShader::VulkanGraphicsShader(
            const VkDevice& device)
            : VulkanShader(device)
        {
            m_shaderPath = DEFAULT_VULKAN_SHADER_PATH;

            m_shaderSources.resize(BX_MAX_SHADER);

            m_shaderFlags.value = 0;
        }

        VulkanGraphicsShader::~VulkanGraphicsShader()
        {
        }

        const std::vector<VkPipelineShaderStageCreateInfo>& VulkanGraphicsShader::createPipelineShaderStages(
            const BxShaderMeta& shaderMeta)
        {
            // Vertex Shader
            if (shaderMeta.vertexShaderInfo.shaderFile.empty() == FALSE)
            {
                const std::string& vsFile = m_shaderPath + shaderMeta.vertexShaderInfo.shaderFile;

                m_shaderFlags.vertexShaderBit = TRUE;

                createShaderModule(vsFile, BX_VERTEX_SHADER);

                m_shaderStages.push_back(
                    createShaderStage(
                        m_shaderSources[BX_VERTEX_SHADER].shaderModule,
                        VK_SHADER_STAGE_VERTEX_BIT,
                        shaderMeta.vertexShaderInfo.shaderEntry));
            }

            // Tessellation
            if (shaderMeta.tesShaderInfo.shaderFile.empty() == FALSE)
            {
                const std::string& tesFile = m_shaderPath + shaderMeta.tesShaderInfo.shaderFile;

                m_shaderFlags.tesShaderBit = TRUE;

                createShaderModule(tesFile, BX_TES_SHADER);

                m_shaderStages.push_back(
                    createShaderStage(
                        m_shaderSources[BX_TES_SHADER].shaderModule,
                        VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
                        shaderMeta.tesShaderInfo.shaderEntry));
            }

            if (shaderMeta.tcsShaderInfo.shaderFile.empty() == FALSE)
            {
                const std::string& tcsFile = m_shaderPath + shaderMeta.tcsShaderInfo.shaderFile;

                m_shaderFlags.tcsShaderBit = TRUE;

                createShaderModule(tcsFile, BX_TCS_SHADER);

                m_shaderStages.push_back(
                    createShaderStage(
                        m_shaderSources[BX_TCS_SHADER].shaderModule,
                        VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
                        shaderMeta.tcsShaderInfo.shaderEntry));
            }

            // Geometry shader
            if (shaderMeta.geometryShaderInfo.shaderFile.empty() == FALSE)
            {
                const std::string& gsFile = m_shaderPath + shaderMeta.geometryShaderInfo.shaderFile;

                m_shaderFlags.geometryShaderBit = TRUE;

                createShaderModule(gsFile, BX_GEOMETRY_SHADER);

                m_shaderStages.push_back(
                    createShaderStage(
                        m_shaderSources[BX_GEOMETRY_SHADER].shaderModule,
                        VK_SHADER_STAGE_GEOMETRY_BIT,
                        shaderMeta.geometryShaderInfo.shaderEntry));
            }

            // Fragment(Pixel) shader
            if (shaderMeta.fragmentShaderInfo.shaderFile.empty() == FALSE)
            {
                const std::string& fsFile = m_shaderPath + shaderMeta.fragmentShaderInfo.shaderFile;

                m_shaderFlags.fragmentShaderBit = TRUE;

                createShaderModule(fsFile, BX_FRAGMENT_SHADER);

                m_shaderStages.push_back(
                    createShaderStage(
                        m_shaderSources[BX_FRAGMENT_SHADER].shaderModule,
                        VK_SHADER_STAGE_FRAGMENT_BIT,
                        shaderMeta.fragmentShaderInfo.shaderEntry));
            }

            return m_shaderStages;
        }

        void VulkanGraphicsShader::clean()
        {
            m_shaderSources.clear();
            m_shaderStages.clear();

            m_shaderSources.resize(BX_MAX_SHADER);
        }

        void VulkanGraphicsShader::createShaderModule(
            const std::string&   shaderFile,
            const BX_SHADER_TYPE shaderType)
        {
            m_shaderSources[shaderType] =
            {
                shaderType,
                Utility::VulkanUtility::ReadFile(shaderFile, TRUE),
                { m_device, vkDestroyShaderModule }
            };

            VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
            shaderModuleCreateInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            shaderModuleCreateInfo.codeSize = m_shaderSources[shaderType].source.size();
            shaderModuleCreateInfo.pCode    = reinterpret_cast<const UINT*>(
                m_shaderSources[shaderType].source.data());

            VkResult result = vkCreateShaderModule(
                m_device, &shaderModuleCreateInfo, NULL, m_shaderSources[shaderType].shaderModule.replace());

            assert(result == VK_SUCCESS);
        }

        VkPipelineShaderStageCreateInfo VulkanGraphicsShader::createShaderStage(
            const VkShaderModule&       shaderModule,
            const VkShaderStageFlagBits stageBit,
            const std::string&          shaderEntry)
        {
            VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
            shaderStageCreateInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStageCreateInfo.stage  = stageBit;
            shaderStageCreateInfo.module = shaderModule;
            shaderStageCreateInfo.pName  = shaderEntry.data();

            return shaderStageCreateInfo;
        }
    }
}