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

            m_isScenePipeline                              = pGraphicsPipelineCreateData->isScenePipeline;

            // Bind vertex/index buffer
            m_pVertexInputResourceList = pResource->pVertexInputResourceList;

            /// Setup Fixed pipeline stages
            // VS input
            VkPipelineVertexInputStateCreateInfo vsInputCreateInfo = {};
            vsInputCreateInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vsInputCreateInfo.vertexBindingDescriptionCount   = 1;
            vsInputCreateInfo.pVertexBindingDescriptions      = pGraphicsPipelineCreateData->pVertexInputBindingDescription;
            vsInputCreateInfo.vertexAttributeDescriptionCount =
                static_cast<UINT>(pGraphicsPipelineCreateData->pVertexInputAttributeDescriptionList->size());
            vsInputCreateInfo.pVertexAttributeDescriptions    =
                pGraphicsPipelineCreateData->pVertexInputAttributeDescriptionList->data();

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
            multiSamplingCreateInfo.rasterizationSamples = Utility::VulkanUtility::GetVkSampleCount(pProps->samples);
            multiSamplingCreateInfo.sampleShadingEnable  = VK_FALSE;

            // Blending states
            const std::vector<VulkanGraphicsPipelineRenderTargetProperties>* pRenderTargetsProps =
                pProps->pRenderTargetsProps;

            const UINT renderTargetNum = static_cast<UINT>(pProps->pRenderTargetsProps->size());

            std::vector<VkPipelineColorBlendAttachmentState> blendAttachmentStateList(renderTargetNum);

            VkPipelineColorBlendStateCreateInfo blendState = {};
            blendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

            if (pGraphicsPipelineCreateData->enableColor == TRUE)
            {
                for (UINT RTIndex = 0; RTIndex < renderTargetNum; ++RTIndex)
                {
                    VkPipelineColorBlendAttachmentState* pBlendAttachmentState = &(blendAttachmentStateList[RTIndex]);

                    pBlendAttachmentState->colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                                            VK_COLOR_COMPONENT_G_BIT |
                                                            VK_COLOR_COMPONENT_B_BIT |
                                                            VK_COLOR_COMPONENT_A_BIT;

                    if (pRenderTargetsProps->at(RTIndex).enableBlend == TRUE)
                    {
                        pBlendAttachmentState->blendEnable         = VK_TRUE;
                        pBlendAttachmentState->alphaBlendOp        = VK_BLEND_OP_ADD;
                        pBlendAttachmentState->srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                        pBlendAttachmentState->dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
                        pBlendAttachmentState->alphaBlendOp        = VK_BLEND_OP_ADD;
                        pBlendAttachmentState->srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                        pBlendAttachmentState->dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                    }
                    else
                    {
                        pBlendAttachmentState->blendEnable = VK_FALSE;
                    }
                }

                blendState.attachmentCount = renderTargetNum;
                blendState.pAttachments    = blendAttachmentStateList.data();
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
            const UINT descriptorSetNum =
                static_cast<UINT>(pResource->pDescriptorResourceList->size());

            std::vector<VDeleter<VkDescriptorSetLayout>>
                descriptorLayoutList(descriptorSetNum,
                                     { *m_pDevice, vkDestroyDescriptorSetLayout });

            std::vector<UINT> descriptorSetIndexList(descriptorSetNum);

            std::vector<std::vector<Mgr::DescriptorCreateInfo>> descriptorCreateInfoTable(descriptorSetNum);

            for (UINT descriptorResourceIndex = 0;
                 descriptorResourceIndex < descriptorSetNum;
                 ++descriptorResourceIndex)
            {
                const VulkanDescriptorResources* descriptorResources =
                    &(pResource->pDescriptorResourceList->at(descriptorResourceIndex));

                const UINT descriptorSetIndex = descriptorResources->descriptorSetIndex;

                size_t uniformBufferDescriptorNum   = ((descriptorResources->pUniformBufferResourceList != NULL) ?
                                                       descriptorResources->pUniformBufferResourceList->size() : 0);

                size_t textureDescriptorNum         = ((descriptorResources->pTextureResouceList != NULL) ?
                                                       descriptorResources->pTextureResouceList->size() : 0);

                size_t inputAttachmentDescriptorNum = ((descriptorResources->pInputAttachmentList != NULL) ?
                                                       descriptorResources->pInputAttachmentList->size() : 0);

                size_t descriptorTotalNum           = uniformBufferDescriptorNum +
                                                      textureDescriptorNum       +
                                                      inputAttachmentDescriptorNum;

                if (descriptorTotalNum > 0)
                {
                    std::vector<Mgr::DescriptorCreateInfo>* pDescriptorCreateInfoList =
                        &(descriptorCreateInfoTable[descriptorResourceIndex]);

                    pDescriptorCreateInfoList->resize(descriptorTotalNum);

                    m_uniformBufferDescriptorUpdateInfo.resize(uniformBufferDescriptorNum);
                    m_textureDescriptorUpdateInfo.resize(textureDescriptorNum);
                    m_inputAttachmentUpdateInfo.resize(inputAttachmentDescriptorNum);

                    size_t descriptorCounter = 0;

                    for (UINT i = 0; i < uniformBufferDescriptorNum; ++i)
                    {
                        Mgr::DescriptorCreateInfo* pDescriptorCreateInfo            = &(pDescriptorCreateInfoList->at(i));
                        Mgr::DescriptorUpdateInfo* pDescriptorUpdateInfo            = &(m_uniformBufferDescriptorUpdateInfo[i]);
                        Render::VulkanUniformBufferResource* pUniformbufferResource =
                            &(descriptorResources->pUniformBufferResourceList->at(i));

                        BX_DESCRIPTOR_TYPE uniformDescriptorType =
                            (pUniformbufferResource->pUniformBuffer->IsBufferDynamic() ? BX_UNIFORM_DESCRIPTOR_DYNAMIC :
                                                                                         BX_UNIFORM_DESCRIPTOR);

                        pDescriptorCreateInfo->descriptorType = uniformDescriptorType;
                        pDescriptorCreateInfo->bindingPoint   = pUniformbufferResource->bindingPoint;
                        pDescriptorCreateInfo->descriptorNum  = pUniformbufferResource->uniformbufferNum;
                        pDescriptorCreateInfo->shaderType     = pUniformbufferResource->shaderType;

                        pDescriptorUpdateInfo->descriptorType         = uniformDescriptorType;
                        pDescriptorUpdateInfo->descriptorSetIndex     = pUniformbufferResource->setIndex;
                        pDescriptorUpdateInfo->descriptorBindingIndex = pUniformbufferResource->bindingPoint;
                        pDescriptorUpdateInfo->pDescriptorBuffer      = pUniformbufferResource->pUniformBuffer;
                        pDescriptorUpdateInfo->pDescriptorTexture     = NULL;
                    }

                    descriptorCounter += uniformBufferDescriptorNum;

                    for (UINT i = 0; i < textureDescriptorNum; ++i)
                    {
                        Mgr::DescriptorCreateInfo* pDescriptorCreateInfo = &(pDescriptorCreateInfoList->at(i + descriptorCounter));
                        Mgr::DescriptorUpdateInfo* pDescriptorUpdateInfo = &(m_textureDescriptorUpdateInfo[i]);
                        Render::VulkanTextureResource* pTexResource      =
                            &(descriptorResources->pTextureResouceList->at(i));

                        pDescriptorCreateInfo->descriptorType = BX_SAMPLER_DESCRIPTOR;
                        pDescriptorCreateInfo->bindingPoint   = pTexResource->bindingPoint;
                        pDescriptorCreateInfo->descriptorNum  = pTexResource->textureNum;
                        pDescriptorCreateInfo->shaderType     = pTexResource->shaderType;

                        pDescriptorUpdateInfo->descriptorType         = BX_SAMPLER_DESCRIPTOR;
                        pDescriptorUpdateInfo->descriptorSetIndex     = pTexResource->setIndex;
                        pDescriptorUpdateInfo->descriptorBindingIndex = pTexResource->bindingPoint;
                        pDescriptorUpdateInfo->pDescriptorBuffer      = NULL;
                        pDescriptorUpdateInfo->pDescriptorTexture     = pTexResource->pTexture;
                    }

                    descriptorCounter += textureDescriptorNum;

                    for (UINT inputAttachmentIndex = 0;
                         inputAttachmentIndex < inputAttachmentDescriptorNum;
                         ++inputAttachmentIndex)
                    {
                        Mgr::DescriptorCreateInfo* pInputAttachmentDescriptorCreateInfo =
                            &(pDescriptorCreateInfoList->at(inputAttachmentIndex + descriptorCounter));

                        Mgr::DescriptorUpdateInfo* pInputAttachmentDescriptorUpdateInfo =
                            &(m_inputAttachmentUpdateInfo[inputAttachmentIndex]);

                        Render::VulkanTextureResource* pInputAttachmentDescriptorResource =
                            &(descriptorResources->pInputAttachmentList->at(inputAttachmentIndex));

                        pInputAttachmentDescriptorCreateInfo->descriptorType = BX_INPUT_ATTACHMENT_DESCRIPTOR;
                        pInputAttachmentDescriptorCreateInfo->bindingPoint   = pInputAttachmentDescriptorResource->bindingPoint;
                        pInputAttachmentDescriptorCreateInfo->descriptorNum  = 1;
                        pInputAttachmentDescriptorCreateInfo->shaderType     = pInputAttachmentDescriptorResource->shaderType;

                        pInputAttachmentDescriptorUpdateInfo->descriptorType         = BX_INPUT_ATTACHMENT_DESCRIPTOR;
                        pInputAttachmentDescriptorUpdateInfo->descriptorSetIndex     = pInputAttachmentDescriptorResource->setIndex;
                        pInputAttachmentDescriptorUpdateInfo->descriptorBindingIndex = pInputAttachmentDescriptorResource->bindingPoint;
                        pInputAttachmentDescriptorUpdateInfo->pDescriptorBuffer      = NULL;
                        pInputAttachmentDescriptorUpdateInfo->pDescriptorTexture     = pInputAttachmentDescriptorResource->pTexture;
                    }

                    descriptorCounter += inputAttachmentDescriptorNum;

                    assert(descriptorCounter == descriptorTotalNum);

                    descriptorLayoutList[descriptorResourceIndex] =
                        m_pDescriptorMgr->createDescriptorSetLayout(*pDescriptorCreateInfoList);

                    descriptorSetIndexList[descriptorResourceIndex] = descriptorSetIndex;
                }

                status = m_pDescriptorMgr->
                    createDescriptorSets(
                        VDeleter<VkDescriptorSetLayout>::GetRawVector(descriptorLayoutList),
                        descriptorSetIndexList);

                assert(status = BX_SUCCESS);

                if (m_uniformBufferDescriptorUpdateInfo.size() > 0)
                {
                    status = m_pDescriptorMgr->updateDescriptorSet(m_uniformBufferDescriptorUpdateInfo);
                }

                if (m_textureDescriptorUpdateInfo.size() > 0)
                {
                    status = m_pDescriptorMgr->updateDescriptorSet(m_textureDescriptorUpdateInfo);
                }

                if (m_inputAttachmentUpdateInfo.size() > 0)
                {
                    status = m_pDescriptorMgr->updateDescriptorSet(m_inputAttachmentUpdateInfo);
                }

                assert(status = BX_SUCCESS);
            }

            VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
            pipelineLayoutCreateInfo.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

            std::vector<VkDescriptorSetLayout> descriptorLayoutRawList;

            if (descriptorLayoutList.size() > 0)
            {
                descriptorLayoutRawList =
                    VDeleter<VkDescriptorSetLayout>::GetRawVector(descriptorLayoutList);

                pipelineLayoutCreateInfo.setLayoutCount = descriptorSetNum;
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
                graphicsPipelineCreateInfo.subpass             = pGraphicsPipelineCreateData->subpassIndex;

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
