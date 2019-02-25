//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2019
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "VulkanGraphicsPipeline.h"

namespace VulkanEngine
{
    namespace Render
    {
        VulkanGraphicsPipeline::VulkanGraphicsPipeline(
            const Setting*      const pSetting,
            const VkDevice*     const pDevice,
            const VkRenderPass* const pRenderPass,
            Mgr::CmdBufferMgr*  const pCmdBufferMgr,
            Mgr::DescriptorMgr* const pDescritorMgr)
            : m_pSetting(pSetting),
              m_pDevice(pDevice),
              m_pRenderPass(pRenderPass),
              m_pCmdBufferMgr(pCmdBufferMgr),
              m_pDescriptorMgr(pDescritorMgr),
              m_shader(*pDevice)
        {
            m_graphicsPipeline       = { *m_pDevice, vkDestroyPipeline       };
            m_graphicsPipelineLayout = { *m_pDevice, vkDestroyPipelineLayout };
        }

        VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
        {
        }

        BOOL VulkanGraphicsPipeline::createGraphicsPipeline(
            VulkanGraphicsPipelineCreateData* pGraphicsPipelineCreateData)
        {
            BOOL status = BX_SUCCESS;

            VulkanGraphicsPipelineProperties* const pProps = pGraphicsPipelineCreateData->pProps;
            Shader::BxShaderMeta*   const pShaderMeta      = pGraphicsPipelineCreateData->pShaderMeta;
            VulkanRenderResources*  const pResource        = pGraphicsPipelineCreateData->pResource;

            // Bind vertex/index buffer
            m_pVertexInputResourceList = pResource->pVertexInputResourceList;

            /// Setup Fixed pipeline stages
            // VS input
            VkVertexInputBindingDescription bindingDescription =
                Buffer::VulkanVertexBuffer::createDescription(pResource->vertexDescriptionBindingPoint,
                                                              BX_VERTEX_INPUT_RATE_VERTEX);

            std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;

            if (pResource->vertexBufferTexChannelNum <= 1)
            {
                vertexInputAttributeDescriptions =
                    Buffer::VulkanVertexBuffer::
                        createAttributeDescriptions(pResource->vertexDescriptionBindingPoint);
            }
            else
            {
                vertexInputAttributeDescriptions =
                    Buffer::VulkanVertexBuffer::
                        createAttributeDescriptionsMultipleTexture(pResource->vertexDescriptionBindingPoint,
                                                                   pResource->vertexBufferTexChannelNum);
            }

            VkPipelineVertexInputStateCreateInfo vsInputCreateInfo = {};
            vsInputCreateInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vsInputCreateInfo.vertexBindingDescriptionCount   = 1;
            vsInputCreateInfo.pVertexBindingDescriptions      = &bindingDescription;
            vsInputCreateInfo.vertexAttributeDescriptionCount = static_cast<UINT>(vertexInputAttributeDescriptions.size());
            vsInputCreateInfo.pVertexAttributeDescriptions    = vertexInputAttributeDescriptions.data();

            // Input assembly state
            VkPipelineInputAssemblyStateCreateInfo inputAsmCreateInfo = {};
            inputAsmCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAsmCreateInfo.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAsmCreateInfo.primitiveRestartEnable = VK_FALSE;

            // Rasterizer
            VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {};
            rasterizerCreateInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizerCreateInfo.depthClampEnable        = VK_FALSE;
            rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
            rasterizerCreateInfo.polygonMode             = Utility::VulkanUtility::GetVkPolygonMode(pProps->polyMode);
            rasterizerCreateInfo.lineWidth               = 1.0f;
            rasterizerCreateInfo.cullMode                = Utility::VulkanUtility::GetVkCullMode(pProps->cullMode);
            rasterizerCreateInfo.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            rasterizerCreateInfo.depthBiasEnable         = VK_FALSE;

            // Multisampling
            VkPipelineMultisampleStateCreateInfo multiSamplingCreateInfo = {};
            multiSamplingCreateInfo.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multiSamplingCreateInfo.rasterizationSamples =
                Utility::VulkanUtility::GetVkSampleCount(m_pSetting->m_graphicsSetting.antialasing);
            multiSamplingCreateInfo.sampleShadingEnable  = VK_FALSE;

            VkPipelineColorBlendAttachmentState blendAttachmentState = {};

            VkPipelineColorBlendStateCreateInfo blendState = {};
            blendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

            if (pGraphicsPipelineCreateData->enableColor == TRUE)
            {
                blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                                      VK_COLOR_COMPONENT_G_BIT |
                                                      VK_COLOR_COMPONENT_B_BIT |
                                                      VK_COLOR_COMPONENT_A_BIT;

                if (m_pSetting->m_graphicsSetting.blend == TRUE)
                {
                    blendAttachmentState.blendEnable         = VK_TRUE;
                    blendAttachmentState.alphaBlendOp        = VK_BLEND_OP_ADD;
                    blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                    blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
                    blendAttachmentState.alphaBlendOp        = VK_BLEND_OP_ADD;
                    blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                    blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                }
                else
                {
                    blendAttachmentState.blendEnable = VK_FALSE;
                }

                blendState.attachmentCount = 1;
                blendState.pAttachments    = &blendAttachmentState;
                blendState.logicOpEnable   = VK_FALSE; // Enable will disable the states in pAttachments
            }

            VkPipelineDepthStencilStateCreateInfo* pDepthStencilState = NULL;
            VkPipelineDepthStencilStateCreateInfo  depthStencilState = {};
            depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

            if (pGraphicsPipelineCreateData->enableDepth == TRUE)
            {
                depthStencilState.depthTestEnable       = VK_TRUE;
                depthStencilState.depthWriteEnable      = VK_TRUE;
                depthStencilState.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
                depthStencilState.depthBoundsTestEnable = VK_FALSE;
                depthStencilState.minDepthBounds        = 0.0f;
                depthStencilState.maxDepthBounds        = 1.0f;
                depthStencilState.stencilTestEnable     = VK_FALSE;

                pDepthStencilState = &depthStencilState;
            }

            if (pGraphicsPipelineCreateData->enableStencil == TRUE)
            {
                NotImplemented();

                depthStencilState.stencilTestEnable = VK_TRUE;
                depthStencilState.front             = {};
                depthStencilState.back              = {};

                pDepthStencilState = &depthStencilState;
            }

            // Viewport and scissor
            size_t viewportNum = pProps->viewportRects.size();
            size_t scissorNum  = pProps->scissorRects.size();

            assert(viewportNum == scissorNum);

            std::vector<VkViewport> renderViewports(viewportNum);
            std::vector<VkRect2D>   renderScissors(scissorNum);

            for (size_t i = 0; i < viewportNum; ++i)
            {
                Rectangle* pViewportRect = &(pProps->viewportRects[i]);

                float viewportWidth  = ((pViewportRect->right) - (pViewportRect->left));
                float viewportHeight = ((pViewportRect->bottom) - (pViewportRect->top));

                assert(viewportWidth > 0 && viewportHeight > 0);

                VkExtent2D renderExtent =
                {
                    static_cast<UINT>(viewportWidth),
                    static_cast<UINT>(viewportHeight)
                };

                VkViewport viewport = {};
                viewport.x          = pViewportRect->left;
                viewport.y          = pViewportRect->top;
                viewport.width      = viewportWidth;
                viewport.height     = viewportHeight;
                viewport.minDepth   = 0.0f;
                viewport.maxDepth   = 1.0f;

                renderViewports[i] = viewport;

                Rectangle* pScissorRect = &(pProps->scissorRects[i]);

                float scissorWidth  = ((pScissorRect->right) - (pScissorRect->left));
                float scissorHeight = ((pScissorRect->bottom) - (pScissorRect->top));

                VkExtent2D scissorExtent =
                {
                    static_cast<UINT>(scissorWidth),
                    static_cast<UINT>(scissorHeight)
                };

                VkRect2D scissor = {};
                scissor.extent   = scissorExtent;
                scissor.offset   =
                {
                    static_cast<INT>(pScissorRect->left),
                    static_cast<INT>(pScissorRect->top)
                };

                renderScissors[i] = scissor;
            }

            VkPipelineViewportStateCreateInfo viewportCreateInfo = {};
            viewportCreateInfo.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportCreateInfo.viewportCount = static_cast<UINT>(viewportNum);
            viewportCreateInfo.pViewports    = renderViewports.data();
            viewportCreateInfo.scissorCount  = static_cast<UINT>(scissorNum);
            viewportCreateInfo.pScissors     = renderScissors.data();

            /// Pipeline Layout

            // Create descriptors
            size_t uniformBufferDescriptorNum = ((pResource->pUniformBufferResourceList != NULL) ?
                                                  pResource->pUniformBufferResourceList->size() : 0);

            size_t textureDescriptorNum       = ((pResource->pTextureResouceList != NULL) ?
                                                  pResource->pTextureResouceList->size() : 0);

            size_t descriptorTotalNum         = uniformBufferDescriptorNum + textureDescriptorNum;

            std::vector<VDeleter<VkDescriptorSetLayout>> descriptorLayoutList;
            if (descriptorTotalNum > 0)
            {
                std::vector<Mgr::DescriptorCreateInfo> descriptorCreateInfo(descriptorTotalNum);

                m_uniformBufferDescriptorUpdateInfo.resize(uniformBufferDescriptorNum);
                m_textureDescriptorUpdateInfo.resize(textureDescriptorNum);

                UINT descriptorCounter = 0;

                for (UINT i = 0; i < uniformBufferDescriptorNum; ++i)
                {
                    Mgr::DescriptorCreateInfo* pDescriptorCreateInfo            = &(descriptorCreateInfo[i]);
                    Mgr::DescriptorUpdateInfo* pDescriptorUpdateInfo            = &(m_uniformBufferDescriptorUpdateInfo[i]);
                    Render::VulkanUniformBufferResource* pUniformbufferResource = &(pResource->pUniformBufferResourceList->at(i));

                    pDescriptorCreateInfo->descriptorType = BX_UNIFORM_DESCRIPTOR_DYNAMIC;
                    pDescriptorCreateInfo->bindingPoint   = pUniformbufferResource->bindingPoint;
                    pDescriptorCreateInfo->descriptorNum  = pUniformbufferResource->uniformbufferNum;
                    pDescriptorCreateInfo->shaderType     = pUniformbufferResource->shaderType;

                    pDescriptorUpdateInfo->descriptorType         = BX_UNIFORM_DESCRIPTOR_DYNAMIC;
                    pDescriptorUpdateInfo->descriptorSetIndex     = pUniformbufferResource->setIndex;
                    pDescriptorUpdateInfo->descriptorBindingIndex = pUniformbufferResource->bindingPoint;
                    pDescriptorUpdateInfo->pDescriptorBuffer      = pUniformbufferResource->pUniformBuffer;
                    pDescriptorUpdateInfo->pDescriptorTexture     = NULL;

                    descriptorCounter++;
                }

                for (UINT i = 0; i < textureDescriptorNum; ++i)
                {
                    Mgr::DescriptorCreateInfo* pDescriptorCreateInfo = &(descriptorCreateInfo[i + descriptorCounter]);
                    Mgr::DescriptorUpdateInfo* pDescriptorUpdateInfo = &(m_textureDescriptorUpdateInfo[i]);
                    Render::VulkanTextureResource* pTexResource      = &(pResource->pTextureResouceList->at(i));

                    pDescriptorCreateInfo->descriptorType = BX_SAMPLER_DESCRIPTOR;
                    pDescriptorCreateInfo->bindingPoint   = pTexResource->bindingPoint;
                    pDescriptorCreateInfo->descriptorNum  = pTexResource->textureNum;
                    pDescriptorCreateInfo->shaderType     = pTexResource->shaderType;

                    pDescriptorUpdateInfo->descriptorType         = BX_SAMPLER_DESCRIPTOR;
                    pDescriptorUpdateInfo->descriptorSetIndex     = pTexResource->setIndex;
                    pDescriptorUpdateInfo->descriptorBindingIndex = pTexResource->bindingPoint;
                    pDescriptorUpdateInfo->pDescriptorBuffer      = NULL;
                    pDescriptorUpdateInfo->pDescriptorTexture     = pTexResource->pTexture;

                    descriptorCounter++;
                }

                descriptorLayoutList.resize(pGraphicsPipelineCreateData->renderTargetNum,
                                            { *m_pDevice, vkDestroyDescriptorSetLayout });

                for (size_t i = 0; i < pGraphicsPipelineCreateData->renderTargetNum; ++i)
                {
                    descriptorLayoutList[i] = m_pDescriptorMgr->createDescriptorSetLayout(descriptorCreateInfo);
                }

                std::vector<Mgr::DescriptorPoolCreateInfo> descriptorPoolCreateData(2);
                descriptorPoolCreateData[0].descriptorType = BX_UNIFORM_DESCRIPTOR;
                descriptorPoolCreateData[0].descriptorNum  = static_cast<UINT>(uniformBufferDescriptorNum);

                descriptorPoolCreateData[1].descriptorType = BX_SAMPLER_DESCRIPTOR;
                descriptorPoolCreateData[1].descriptorNum  = static_cast<UINT>(textureDescriptorNum);

                status = m_pDescriptorMgr->createDescriptorPool(descriptorPoolCreateData);
                assert(status == BX_SUCCESS);

                std::vector<UINT> descriptorSetIndexList(pGraphicsPipelineCreateData->renderTargetNum);
                for (size_t i = 0; i < pGraphicsPipelineCreateData->renderTargetNum; ++i)
                {
                    descriptorSetIndexList[i] = static_cast<UINT>(i);
                }

                status = m_pDescriptorMgr->
                    createDescriptorSets(VDeleter<VkDescriptorSetLayout>::GetRawVector(descriptorLayoutList),
                                         descriptorSetIndexList);

                assert(status = BX_SUCCESS);

                status = m_pDescriptorMgr->updateDescriptorSet(m_uniformBufferDescriptorUpdateInfo);
                status = m_pDescriptorMgr->updateDescriptorSet(m_textureDescriptorUpdateInfo);

                assert(status = BX_SUCCESS);
            }

            VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
            pipelineLayoutCreateInfo.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

            std::vector<VkDescriptorSetLayout> descriptorLayoutRawList;

            if (descriptorLayoutList.size() > 0)
            {
                descriptorLayoutRawList =
                    VDeleter<VkDescriptorSetLayout>::GetRawVector(descriptorLayoutList);

                pipelineLayoutCreateInfo.setLayoutCount = static_cast<UINT>(pGraphicsPipelineCreateData->renderTargetNum);
                pipelineLayoutCreateInfo.pSetLayouts    = descriptorLayoutRawList.data();
            }

            VkResult vkResult = vkCreatePipelineLayout(
                *m_pDevice, &pipelineLayoutCreateInfo, NULL, m_graphicsPipelineLayout.replace());

            status = Utility::VulkanUtility::GetBxStatus(vkResult);

            assert(status == BX_SUCCESS);

            /// End fixed pipeline stages setup

            /// Setup programmable pipeline stages
            std::vector<VkPipelineShaderStageCreateInfo> shaderCreateInfo;

            if (status == BX_SUCCESS)
            {
                shaderCreateInfo =
                    m_shader.createPipelineShaderStages(*pShaderMeta);
            }
            /// End programmable pipeline stages setup

            // Create pipeline
            if (status == BX_SUCCESS)
            {
                VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
                graphicsPipelineCreateInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
                graphicsPipelineCreateInfo.pVertexInputState   = &vsInputCreateInfo;
                graphicsPipelineCreateInfo.pInputAssemblyState = &inputAsmCreateInfo;
                graphicsPipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
                graphicsPipelineCreateInfo.pMultisampleState   = &multiSamplingCreateInfo;
                graphicsPipelineCreateInfo.pColorBlendState    = &blendState;
                graphicsPipelineCreateInfo.pDepthStencilState  = pDepthStencilState;
                graphicsPipelineCreateInfo.pViewportState      = &viewportCreateInfo;
                graphicsPipelineCreateInfo.layout              = m_graphicsPipelineLayout;
                graphicsPipelineCreateInfo.renderPass          = *m_pRenderPass;
                graphicsPipelineCreateInfo.stageCount          = static_cast<UINT>(shaderCreateInfo.size());
                graphicsPipelineCreateInfo.pStages             = shaderCreateInfo.data();
                graphicsPipelineCreateInfo.pDynamicState       = NULL;
                graphicsPipelineCreateInfo.subpass             = 0;

                VkResult graphicsPipelineCreateResult = vkCreateGraphicsPipelines(*m_pDevice,
                                                                                  VK_NULL_HANDLE,
                                                                                  1,
                                                                                  &graphicsPipelineCreateInfo,
                                                                                  NULL,
                                                                                  m_graphicsPipeline.replace());

                status = ((graphicsPipelineCreateResult == VK_SUCCESS) ? BX_SUCCESS : BX_FAIL);

                assert(status == BX_SUCCESS);
            }

            return status;
        }

        BOOL VulkanGraphicsPipeline::update(
            const float                                    deltaTime,
            const std::vector<VulkanDescriptorUpdateData>& updateDataList)
        {
            BOOL status = BX_SUCCESS;

            size_t updateSize = updateDataList.size();
            for (size_t i = 0; i < updateSize; ++i)
            {
                Buffer::VulkanDescriptorBuffer* pDescriptorBuffer =
                    m_uniformBufferDescriptorUpdateInfo[i].pDescriptorBuffer;

                status = pDescriptorBuffer->updateBufferData(pDescriptorBuffer->GetBufferSize(),
                    updateDataList[i].pData);
            }

            assert(status == BX_SUCCESS);

            return status;
        }
    }
}
