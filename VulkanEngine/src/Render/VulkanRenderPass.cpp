#include "VulkanRenderPass.h"
//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

namespace VulkanEngine
{
    namespace Render
    {
        VulkanRenderPass::VulkanRenderPass(
            const Setting*      const pSetting,
            const VkDevice*     const pDevice,
            Mgr::CmdBufferMgr*  const pCmdBufferMgr,
            Mgr::DescriptorMgr* const pDescritorMgr,
            const Scene::RenderScene* pScene)
            : m_pSetting(pSetting),
              m_pDevice(pDevice),
              m_pCmdBufferMgr(pCmdBufferMgr),
              m_pDescriptorMgr(pDescritorMgr),
              m_pScene(pScene),
              m_shader(*pDevice)
        {
            m_renderPass             = { *m_pDevice, vkDestroyRenderPass     };
            m_graphicsPipeline       = { *m_pDevice, vkDestroyPipeline       };
            m_graphicsPipelineLayout = { *m_pDevice, vkDestroyPipelineLayout };
        }

        VulkanRenderPass::~VulkanRenderPass()
        {
        }

        BOOL VulkanRenderPass::create(
            const VulkanRenderPassCreateData& renderPassCreateData)
        {
            BOOL status = BX_SUCCESS;

            status = createRenderTargets(renderPassCreateData.pRenderTargetCreateDataList,
                                         renderPassCreateData.renderSubPassNum,
                                         renderPassCreateData.renderFramebufferNum);

            assert(status == BX_SUCCESS);

            if (status == BX_SUCCESS)
            {
                status = createGraphicsPipeline(renderPassCreateData.pProps,
                                                renderPassCreateData.pShaderMeta,
                                                renderPassCreateData.pResource);

                assert(status == BX_SUCCESS);
            }

            return status;
        }

        BOOL VulkanRenderPass::update()
        {
            BOOL status = BX_SUCCESS;

            // Not support updating textures at real-time

            assert(status == BX_SUCCESS);

            return status;
        }

        BOOL VulkanRenderPass::draw()
        {
            BOOL status = BX_SUCCESS;

            const Scene::RenderScene* pScene = m_pScene;

            const UINT framebufferSize = static_cast<UINT>(m_framebufferList.size());
            const UINT cmdBufferOffset = m_pCmdBufferMgr->GetGraphicsCmdBufferNum();

            status = m_pCmdBufferMgr->
                addGraphicsCmdBuffers(BX_QUEUE_GRAPHICS,
                                      BX_DIRECT_COMMAND_BUFFER,
                                      framebufferSize);

            if (status == BX_FAIL)
            {
                printf("Failed to create graphics command buffers!\n");
                assert(BX_FAIL);
            }

            const size_t framebufferNum = m_framebufferList.size();
            const UINT   cmdBufferEnd   = cmdBufferOffset + static_cast<UINT>(framebufferNum);

            for (UINT i = cmdBufferOffset; i < cmdBufferEnd; ++i)
            {
                Buffer::CmdBuffer* const pCmdBuffer =
                    m_pCmdBufferMgr->GetCmdBuffer(BX_GRAPHICS_COMMAND_BUFFER, i);

                status = pCmdBuffer->beginCmdBuffer(TRUE);

                assert(status == BX_SUCCESS);
                if (status == BX_SUCCESS)
                {
                    pCmdBuffer->beginRenderPass(m_renderPass,
                                                m_framebufferList[i].GetFramebufferHandle(),
                                                m_renderViewport,
                                                { *m_pClearColor });

                    const UINT camNum = pScene->GetSceneCameraNum();
                    for (UINT camIndex = 0; camIndex < camNum; ++camIndex)
                    {
                        Object::Camera::CameraBase* pCam = pScene->GetCamera(camIndex);
                        
                        const UINT vertexInputResourceNum = static_cast<UINT>(m_pVertexInputResourceList->size());
                        UINT vertexInputResourceCounter   = 0;

                        if (pCam->IsEnable() == TRUE)
                        {
                            const Math::Mat4* pViewMat = &(pCam->GetViewMatrix());
                            const Math::Mat4* pProspectMat = &(pCam->GetProjectionMatrix());

                            const Math::Mat4 vpMat = (*pProspectMat) * (*pViewMat);

                            const UINT modelNum = pScene->GetSceneModelNum();

                            std::vector<Math::Mat4> transfromMatList(modelNum * 4);

                            for (UINT modelIndex = 0; modelIndex < modelNum; ++modelIndex)
                            {
                                Object::Model::ModelObject* pModel = pScene->GetModel(modelIndex);

                                if (pModel->IsEnable() == TRUE)
                                {
                                    Math::Mat4 wvpMat                = vpMat * pModel->GetTrans()->GetTransMatrix();
                                    transfromMatList[modelIndex * 4] = wvpMat;
                                }
                            }

                            for (UINT modelIndex = 0; modelIndex < modelNum; ++modelIndex)
                            {
                                Object::Model::ModelObject* pModel = pScene->GetModel(modelIndex);

                                // pModel->GetTrans()->TransPos(Math::Vector3(0.0f, -delta, 0.0f));

                                if (pModel->IsEnable() == TRUE)
                                {
                                    // status = pDescriptorBuffer->updateBufferData(pDescriptorBuffer->GetBufferSize(), &wvpMat);
                                    Buffer::VulkanDescriptorBuffer* pDescriptorBuffer =
                                        m_uniformBufferDescriptorUpdateInfo[0].pDescriptorBuffer;

                                    const UINT dynamicUniformBufferOffset =
                                        modelIndex * static_cast<UINT>(pDescriptorBuffer->GetDescriptorObjectSize());

                                    const UINT meshNum = pModel->GetMeshNum();
                                    for (UINT meshIndex = 0; meshIndex < meshNum; ++meshIndex)
                                    {
                                        VulkanVertexInputResource* pVertexInputResource = &(m_pVertexInputResourceList->at(vertexInputResourceCounter));

                                        pCmdBuffer->cmdBindVertexBuffers({ pVertexInputResource->pVertexBuffer->GetVertexBuffer() },
                                                                         { 0 });

                                        Buffer::VulkanIndexBuffer* pIndexBuffer = pVertexInputResource->pIndexBuffer;

                                        pCmdBuffer->cmdBindIndexBuffers(pIndexBuffer->GetBuffer(),
                                                                        { 0 },
                                                                        pIndexBuffer->GetIndexType());

                                        if (m_pDescriptorMgr->GetDescriptorSetNum() > 0)
                                        {
                                            if (m_pDescriptorMgr->GetDescriptorSet(0) != VK_NULL_HANDLE)
                                            {
                                                /*pCmdBuffer->cmdBindDescriptorSets(m_graphicsPipelineLayout,
                                                                                  { m_pDescriptorMgr->GetDescriptorSet(0) });*/

                                                pCmdBuffer->cmdBindDynamicDescriptorSets(m_graphicsPipelineLayout,
                                                                                         { m_pDescriptorMgr->GetDescriptorSet(0) },
                                                                                         { dynamicUniformBufferOffset });
                                            }
                                        }

                                        pCmdBuffer->cmdDrawElements(m_graphicsPipeline, pIndexBuffer->GetIndexNum(), 0, 0);
                                        assert(status == BX_SUCCESS);

                                        vertexInputResourceCounter++;
                                    }

                                    status = pDescriptorBuffer->updateBufferData(pDescriptorBuffer->GetBufferSize(),
                                                                                 static_cast<void*>(transfromMatList.data()));
                                }
                            }
                        }
                    }

                    pCmdBuffer->endRenderPass();
                }

                pCmdBuffer->endCmdBuffer();
            }

            return status;
        }

        void VulkanRenderPass::clean()
        {
            m_renderPass.replace();
            m_graphicsPipeline.replace();
        }

        BOOL VulkanRenderPass::createRenderTargets(
            const std::vector<VulkanRenderTargetCreateData>* pRenderTargetsCreateDataList,
            const UINT                                       renderSubpassNum,
            UINT                                             renderFramebufferNum)
        {
            BOOL status = BX_SUCCESS;

            size_t renderTargetNum = pRenderTargetsCreateDataList->size();

            std::vector<VkAttachmentDescription>            attachmentDescriptionList(renderTargetNum);
            std::vector<VkSubpassDescription>               subPassDescriptionList(renderSubpassNum);
            std::vector<std::vector<VkAttachmentReference>> colorSubpassAttachmentRefList(renderSubpassNum);
            std::vector<VkAttachmentReference>              depthAttachmentRefList(renderSubpassNum);

            std::vector<std::vector<Texture::VulkanTextureBase*>> pFramebuffersTextureList(renderFramebufferNum);

            /// Create render pass
            for (size_t i = 0; i < renderTargetNum; ++i)
            {
                const VulkanRenderTargetCreateData& renderPassCreateData = pRenderTargetsCreateDataList->at(i);

                VkFormat attachmentFormat = VK_FORMAT_UNDEFINED;

                for (const VulkanRenderTargetFramebufferCreateData& renderTargetFramebufferCreateData :
                    *(renderPassCreateData.pRenderTargetFramebufferCreateData))
                {
                    UINT framebufferIndex            = renderTargetFramebufferCreateData.framebufferIndex;
                    Texture::VulkanTextureBase* pTex = renderTargetFramebufferCreateData.pTexture;

                    pFramebuffersTextureList[framebufferIndex].push_back(pTex);

                    // Validat the texture formats, which should be the same for the same attachment
                    if (attachmentFormat == VK_FORMAT_UNDEFINED)
                    {
                        attachmentFormat = Utility::VulkanUtility::GetVkImageFormat(pTex->GetTextureFormat());
                    }
                    else
                    {
                        VkFormat nextAttachmentFormat = Utility::VulkanUtility::GetVkImageFormat(pTex->GetTextureFormat());

                        if (nextAttachmentFormat != attachmentFormat)
                        {
                            NotSupported();
                        }
                    }
                }

                attachmentDescriptionList[i].format         = attachmentFormat;
                attachmentDescriptionList[i].samples        =
                    Utility::VulkanUtility::GetVkSampleCount(m_pSetting->m_graphicsSetting.antialasing);
                attachmentDescriptionList[i].loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachmentDescriptionList[i].storeOp        = ((renderPassCreateData.isStore == TRUE) ?
                                                              VK_ATTACHMENT_STORE_OP_STORE :
                                                              VK_ATTACHMENT_STORE_OP_DONT_CARE);

                if (renderPassCreateData.useStencil == TRUE)
                {
                    attachmentDescriptionList[i].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                    attachmentDescriptionList[i].stencilStoreOp = ((renderPassCreateData.isStoreStencil) ?
                                                                  VK_ATTACHMENT_STORE_OP_STORE :
                                                                  VK_ATTACHMENT_STORE_OP_DONT_CARE);
                }
                else
                {
                    attachmentDescriptionList[i].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                    attachmentDescriptionList[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                }

                attachmentDescriptionList[i].initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
                attachmentDescriptionList[i].finalLayout    =
                    Utility::VulkanUtility::GetAttachmentVkImageLayout(renderPassCreateData.layout);

                VkAttachmentReference attachmentRef = {};
                attachmentRef.attachment            = renderPassCreateData.bindingPoint;// Output layout index in shader,
                                                                                        // e.g. layout(location = 0) out vec4 outColor
                attachmentRef.layout                =
                    Utility::VulkanUtility::GetAttachmentRefVkImageLayout(renderPassCreateData.layout);

                colorSubpassAttachmentRefList[renderPassCreateData.renderSubPassIndex].push_back(attachmentRef);
            }

            for (UINT i = 0; i < renderSubpassNum; ++i)
            {
                subPassDescriptionList[i].pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
                subPassDescriptionList[i].colorAttachmentCount    = static_cast<UINT>(colorSubpassAttachmentRefList.size());
                subPassDescriptionList[i].pColorAttachments       = colorSubpassAttachmentRefList[i].data();
                //subPassDescriptionList[i].pDepthStencilAttachment = &(depthAttachmentRefList[i]);
            }

            VkRenderPassCreateInfo renderPassCreateInfo = {};
            renderPassCreateInfo.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassCreateInfo.attachmentCount        = static_cast<UINT>(renderTargetNum);
            renderPassCreateInfo.pAttachments           = attachmentDescriptionList.data();
            renderPassCreateInfo.subpassCount           = renderSubpassNum;
            renderPassCreateInfo.pSubpasses             = subPassDescriptionList.data();

            VkResult createRenderpassResult =
                vkCreateRenderPass(*m_pDevice, &renderPassCreateInfo, NULL, m_renderPass.replace());
            status = ((createRenderpassResult == VK_SUCCESS) ? BX_SUCCESS : BX_FAIL);

            assert(status == BX_SUCCESS);

            // Generate framebuffers
            m_framebufferList.resize(renderFramebufferNum, Buffer::VulkanFramebuffer(m_pDevice));

            if (status == BX_SUCCESS)
            {
                for (UINT i = 0; i < renderFramebufferNum; ++i)
                {
                    UINT framebufferWidth  = pFramebuffersTextureList[i][0]->GetTextureWidth();
                    UINT framebufferHeight = pFramebuffersTextureList[i][0]->GetTextureHeight();
                    UINT framebufferLayers = pFramebuffersTextureList[i][0]->GetTextureLayers();

                    Buffer::FramebufferCreateData framebufferCreateData = {};
                    framebufferCreateData.renderPass                    = m_renderPass;
                    framebufferCreateData.ppAttachments                 = &(pFramebuffersTextureList[i]);
                    framebufferCreateData.width                         = framebufferWidth;
                    framebufferCreateData.height                        = framebufferHeight;
                    framebufferCreateData.layers                        = framebufferLayers;

                    m_framebufferList[i].createFramebuffer(framebufferCreateData);
                }
            }

            return status;
        }

        BOOL VulkanRenderPass::createGraphicsPipeline(
            VulkanRenderProperties* const pProps,
            Shader::BxShaderMeta*   const pShaderMeta,
            VulkanRenderResources*  const pResource)
        {
            BOOL status = BX_SUCCESS;

            size_t renderTargetNum = m_framebufferList.size();

            // Initialize draw data
            const Rectangle* pRenderViewportRect = &(pProps->renderViewportRect);

            m_renderViewport.offset =
            {
                static_cast<INT>(pRenderViewportRect->left),
                static_cast<INT>(pRenderViewportRect->top)
            };

            m_renderViewport.extent =
            {
                (static_cast<UINT>(pRenderViewportRect->right) -
                 static_cast<UINT>(pRenderViewportRect->left)),
                (static_cast<UINT>(pRenderViewportRect->bottom) -
                 static_cast<UINT>(pRenderViewportRect->top))
            };

            m_pClearColor = &(pProps->sceneClearValue);

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

            // Depth/Stencil
            VkPipelineColorBlendAttachmentState blendAttachmentState = {};
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

            VkPipelineColorBlendStateCreateInfo blendState = {};
            blendState.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            blendState.attachmentCount = 1;
            blendState.pAttachments    = &blendAttachmentState;
            blendState.logicOpEnable   = VK_FALSE; // Enable will disable the states in pAttachments

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

                    pDescriptorUpdateInfo->descriptorType          = BX_UNIFORM_DESCRIPTOR_DYNAMIC;
                    pDescriptorUpdateInfo->descriptorSetIndex      = 0;
                    pDescriptorUpdateInfo->descriptorBindingIndex  = pUniformbufferResource->bindingPoint;
                    pDescriptorUpdateInfo->pDescriptorBuffer       = pUniformbufferResource->pUniformBuffer;
                    pDescriptorUpdateInfo->pDescriptorTexture      = NULL;

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
                    pDescriptorUpdateInfo->descriptorSetIndex     = 0;
                    pDescriptorUpdateInfo->descriptorBindingIndex = pTexResource->bindingPoint;
                    pDescriptorUpdateInfo->pDescriptorBuffer      = NULL;
                    pDescriptorUpdateInfo->pDescriptorTexture     = pTexResource->pTexture;

                    descriptorCounter++;
                }

                descriptorLayoutList.resize(renderTargetNum, { *m_pDevice, vkDestroyDescriptorSetLayout });
                for (size_t i = 0; i < renderTargetNum; ++i)
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

                std::vector<UINT> descriptorSetIndexList(renderTargetNum);
                for (size_t i = 0; i < renderTargetNum; ++i)
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

                pipelineLayoutCreateInfo.setLayoutCount = static_cast<UINT>(renderTargetNum);
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
                graphicsPipelineCreateInfo.pDepthStencilState  = NULL;
                graphicsPipelineCreateInfo.pViewportState      = &viewportCreateInfo;
                graphicsPipelineCreateInfo.layout              = m_graphicsPipelineLayout;
                graphicsPipelineCreateInfo.renderPass          = m_renderPass;
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
    }
}