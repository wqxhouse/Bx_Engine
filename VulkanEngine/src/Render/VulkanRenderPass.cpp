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
              m_pScene(pScene)
        {
            m_renderPass = { *m_pDevice, vkDestroyRenderPass };
        }

        VulkanRenderPass::~VulkanRenderPass()
        {
        }

        BOOL VulkanRenderPass::create(
            const VulkanRenderpassCreateData& renderPassCreateData)
        {
            BOOL status = BX_SUCCESS;

            std::vector<VulkanRenderSubpassCreateData>* pRenderSubpassCreateDataList = renderPassCreateData.pSubpassCreateDataList;

            const UINT renderSubpassNum = static_cast<UINT>(pRenderSubpassCreateDataList->size());

            std::vector<VkSubpassDescription>               subPassDescriptionList(renderSubpassNum);

            std::vector<VkAttachmentDescription>            attachmentDescriptionList;

            // TODO: Set color/depth/stencil properties for each subpass (not necessary to be the same)
            std::vector<std::vector<VkAttachmentReference>> colorSubpassAttachmentRefList(renderSubpassNum);
            std::vector<VkAttachmentReference>              depthSubpassAttachmentRefList(renderSubpassNum);
            std::vector<std::vector<VkAttachmentReference>> inputSubpassAttachmentRefList(renderSubpassNum);
            std::vector<std::vector<VkAttachmentReference>> resolveSubpassAttachmentRefList(renderSubpassNum);

            std::vector<std::vector<Buffer::VulkanAttachment*>> framebufferAttachmentsTable(renderPassCreateData.framebufferNum);

            const VulkanRenderProperties* pRenderProps = renderPassCreateData.pRenderProperties;

            // Initialize draw data
            const Rectangle* pRenderViewportRect = &(pRenderProps->renderViewportRect);

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

            m_enableColor = pRenderProps->enableColor;
            if (IsColorEnabled() == TRUE)
            {
                const std::vector<VkClearValue>& colorClearValueList = pRenderProps->sceneClearValue;
                m_clearValueList.insert(m_clearValueList.end(), colorClearValueList.begin(), colorClearValueList.end());
            }

            m_enableDepth = pRenderProps->enableDepth;
            if (IsDepthEnabled() == TRUE)
            {
                m_clearValueList.push_back(pRenderProps->depthClearValue);
            }

            m_enableStencil = pRenderProps->enableStencil;
            if (IsStencilEnabled() == TRUE)
            {
                m_clearValueList.push_back(pRenderProps->stencilClearValue);
            }

            size_t renderTargetNum = 0;

            size_t uniformBufferDescriptorNum   = 0;
            size_t textureDescriptorNum         = 0;
            size_t inputAttachmentDescriptorNum = 0;

            std::vector<std::vector<const VulkanDescriptorInfo*>> inputAttachmentDescriptorInfoPtrTable(renderSubpassNum);
            std::vector<std::vector<const VulkanDescriptorInfo*>> resolveAttachmentDescriptorInfoPtrTable(renderSubpassNum);

            // Iterate all subpasses (no necessary follow the subpass index order)
            for (UINT subpassIter = 0; subpassIter < renderSubpassNum; ++subpassIter)
            {
                const VulkanRenderSubpassCreateData& subpassCreateData =
                    pRenderSubpassCreateDataList->at(subpassIter);

                const UINT subpassIndex = subpassCreateData.pSubpassGraphicsPipelineCreateData->subpassIndex;

                const VulkanSubpassGraphicsPipelineCreateData* pSubpassGraphicsPipelineCreateData =
                        subpassCreateData.pSubpassGraphicsPipelineCreateData;

                const std::vector<VulkanDescriptorResources>* pSubpassDescriptorResourcesList =
                    pSubpassGraphicsPipelineCreateData->pResource->pDescriptorResourceList;

                const size_t descriptorSetNum = pSubpassDescriptorResourcesList->size();

                for (size_t descriptorResourceIndex = 0;
                     descriptorResourceIndex < descriptorSetNum;
                     ++descriptorResourceIndex)
                {
                    const VulkanDescriptorResources* descriptorResources =
                        &(pSubpassDescriptorResourcesList->at(descriptorResourceIndex));

                    if (descriptorResources->pUniformBufferResourceList != NULL)
                    {
                        uniformBufferDescriptorNum += descriptorResources->pUniformBufferResourceList->size();
                    }

                    if (descriptorResources->pTextureResouceList != NULL)
                    {
                        textureDescriptorNum += descriptorResources->pTextureResouceList->size();
                    }

                    if (descriptorResources->pInputAttachmentList != NULL)
                    {
                        const std::vector<VulkanTextureResource>* pInputAttachmentResourceList =
                            descriptorResources->pInputAttachmentList;

                        inputAttachmentDescriptorNum += pInputAttachmentResourceList->size();

                        const size_t inputAttachmentResourceNum = pInputAttachmentResourceList->size();

                        for (size_t inputAttchementResourceIndex = 0;
                             inputAttchementResourceIndex < inputAttachmentResourceNum;
                             ++inputAttchementResourceIndex)
                        {
                            inputAttachmentDescriptorInfoPtrTable[subpassIndex].push_back(
                                &(pInputAttachmentResourceList->at(inputAttchementResourceIndex)));
                        }
                    }

                    if (descriptorResources->pResolveAttachmentList != NULL)
                    {
                        const std::vector< VulkanTextureResource>* pResolveAttachmentResourceList =
                            descriptorResources->pResolveAttachmentList;

                        const size_t resolveAttachmentResourceNum = pResolveAttachmentResourceList->size();

                        for (size_t resolveAttachmentResourceIndex = 0;
                             resolveAttachmentResourceIndex < resolveAttachmentResourceNum;
                             ++resolveAttachmentResourceIndex)
                        {
                            resolveAttachmentDescriptorInfoPtrTable[subpassIndex].push_back(
                                &(pResolveAttachmentResourceList->at(resolveAttachmentResourceIndex)));
                        }
                    }
                }

                // Pre calculate the total render target number to prepare for the next iteration
                renderTargetNum += subpassCreateData.pSubpassRenderTargetCreateDataRefList->size();
            }

            // Alloc memory for all attachment descriptions
            attachmentDescriptionList.resize(renderTargetNum);

            // Alloc memory for all render target texture pointers
            for (UINT framebufferIndex = 0; framebufferIndex < renderPassCreateData.framebufferNum; ++framebufferIndex)
            {
                framebufferAttachmentsTable[framebufferIndex].resize(renderTargetNum);
            }

            for (UINT subpassIter = 0; subpassIter < renderSubpassNum; ++subpassIter)
            {
                const VulkanRenderSubpassCreateData& subpassCreateData = pRenderSubpassCreateDataList->at(subpassIter);

                VulkanSubpassGraphicsPipelineCreateData*
                    pSubpassGraphicsPipelineCreateData = subpassCreateData.pSubpassGraphicsPipelineCreateData;

                const UINT subpassIndex                = pSubpassGraphicsPipelineCreateData->subpassIndex;

                const std::vector<const VulkanDescriptorInfo*>* pSubpassinputAttachmentDescriptorInfoPtrList =
                    &(inputAttachmentDescriptorInfoPtrTable[subpassIndex]);

                status = createRenderTargets(subpassCreateData.pSubpassRenderTargetCreateDataRefList,
                                             pSubpassinputAttachmentDescriptorInfoPtrList,
                                             &(resolveAttachmentDescriptorInfoPtrTable[subpassIndex]),
                                             &(subPassDescriptionList[subpassIndex]),
                                             &attachmentDescriptionList,
                                             &(colorSubpassAttachmentRefList[subpassIndex]),
                                             &(depthSubpassAttachmentRefList[subpassIndex]),
                                             &(inputSubpassAttachmentRefList[subpassIndex]),
                                             &(resolveSubpassAttachmentRefList[subpassIndex]),
                                             &(framebufferAttachmentsTable));

                assert(status == BX_SUCCESS);
            }

            status = createRenderPass(renderSubpassNum,
                                      subPassDescriptionList,
                                      attachmentDescriptionList,
                                      renderPassCreateData.pSubpassDependencyList);

            assert(status == BX_SUCCESS);

            m_graphicsPipelineList.resize(renderSubpassNum,
                                          VulkanGraphicsPipeline(m_pSetting,
                                                                 m_pDevice,
                                                                 &m_renderPass,
                                                                 m_pCmdBufferMgr,
                                                                 m_pDescriptorMgr));

            status = createFramebuffers(&(framebufferAttachmentsTable),
                                        renderPassCreateData.framebufferNum);

            // TODO: Support the multipass in a single descriptor pool
            // Create descriptor pool
            std::vector<Mgr::DescriptorPoolCreateInfo> descriptorPoolCreateDataList;

            Mgr::DescriptorPoolCreateInfo uniformDescriptorPoolCreateInfo = {};
            if (uniformBufferDescriptorNum > 0)
            {
                uniformDescriptorPoolCreateInfo.descriptorType = BX_UNIFORM_DESCRIPTOR;
                uniformDescriptorPoolCreateInfo.descriptorNum = static_cast<UINT>(uniformBufferDescriptorNum);

                descriptorPoolCreateDataList.push_back(uniformDescriptorPoolCreateInfo);
            }

            Mgr::DescriptorPoolCreateInfo samplerDescriptorPoolCreateInfo = {};
            if (textureDescriptorNum > 0)
            {
                samplerDescriptorPoolCreateInfo.descriptorType = BX_SAMPLER_DESCRIPTOR;
                samplerDescriptorPoolCreateInfo.descriptorNum = static_cast<UINT>(textureDescriptorNum);

                descriptorPoolCreateDataList.push_back(samplerDescriptorPoolCreateInfo);
            }

            Mgr::DescriptorPoolCreateInfo inputAttachmentDescriptorPoolCreateInfo = {};
            if (inputAttachmentDescriptorNum > 0)
            {
                inputAttachmentDescriptorPoolCreateInfo.descriptorType = BX_INPUT_ATTACHMENT_DESCRIPTOR;
                inputAttachmentDescriptorPoolCreateInfo.descriptorNum = static_cast<UINT>(inputAttachmentDescriptorNum);

                descriptorPoolCreateDataList.push_back(inputAttachmentDescriptorPoolCreateInfo);
            }

            status = m_pDescriptorMgr->createDescriptorPool(descriptorPoolCreateDataList);
            assert(status == BX_SUCCESS);

            // Create graphics pipelines for each subpass
            for (UINT i = 0; i < renderSubpassNum; ++i)
            {
                const VulkanRenderSubpassCreateData& subpassCreateData = pRenderSubpassCreateDataList->at(i);

                const VulkanSubpassGraphicsPipelineCreateData*
                    pSubpassGraphicsPipelineCreateData = subpassCreateData.pSubpassGraphicsPipelineCreateData;

                const UINT subpassIndex = pSubpassGraphicsPipelineCreateData->subpassIndex;

                VulkanGraphicsPipelineCreateData graphicsPipelineCreateData     = {};
                graphicsPipelineCreateData.subpassIndex                         = subpassIndex;
                graphicsPipelineCreateData.enableColor                          = IsColorEnabled();
                graphicsPipelineCreateData.enableDepth                          = IsDepthEnabled();
                graphicsPipelineCreateData.enableStencil                        = IsStencilEnabled();
                graphicsPipelineCreateData.pProps                               = pSubpassGraphicsPipelineCreateData->pProps;
                graphicsPipelineCreateData.pShaderMeta                          = pSubpassGraphicsPipelineCreateData->pShaderMeta;
                graphicsPipelineCreateData.pResource                            = pSubpassGraphicsPipelineCreateData->pResource;
                graphicsPipelineCreateData.isScenePipeline                      = pSubpassGraphicsPipelineCreateData->isScenePipeline;
                graphicsPipelineCreateData.pVertexInputBindingDescription       = pSubpassGraphicsPipelineCreateData->pVertexInputBindingDescription;
                graphicsPipelineCreateData.pVertexInputAttributeDescriptionList =
                    pSubpassGraphicsPipelineCreateData->pVertexInputAttributeDescriptionList;

                status = m_graphicsPipelineList[subpassIndex].createGraphicsPipeline(&graphicsPipelineCreateData);

                assert(status == BX_SUCCESS);
            }

            return status;
        }

        BOOL VulkanRenderPass::update(
            const float                                                 deltaTime,
            const std::vector<std::vector<VulkanDescriptorUpdateData>>& updateDataTable)
        {
            BOOL status = BX_SUCCESS;

            size_t graphicsPipelineNum = m_graphicsPipelineList.size();

            for (size_t pipelineIndex = 0; pipelineIndex < graphicsPipelineNum; ++pipelineIndex)
            {
                status = m_graphicsPipelineList[pipelineIndex].update(deltaTime, updateDataTable[pipelineIndex]);
            }

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
                                                m_clearValueList);

                    const size_t graphicsPipelineNum = m_graphicsPipelineList.size();
                    for (size_t graphicsPipelineIndex = 0; graphicsPipelineIndex < graphicsPipelineNum; ++graphicsPipelineIndex)
                    {
                        const UINT subpassIndex = static_cast<UINT>(graphicsPipelineIndex);

                        const VulkanGraphicsPipeline& graphicsPipeline = m_graphicsPipelineList[subpassIndex];
                        const std::vector<VulkanVertexInputResource>* pVertexInputResourceList =
                            graphicsPipeline.GetVertexInputResourceList();

                        const std::vector<Mgr::DescriptorUpdateInfo>& uniformBufferDescUpdateInfoList =
                            graphicsPipeline.GetUniformBufferDescUpdateInfo();
                        
                        const UINT vertexInputResourceNum = static_cast<UINT>(pVertexInputResourceList->size());

                        if (subpassIndex > 0)
                        {
                            pCmdBuffer->cmdNextSubpass();
                        }

                        if (graphicsPipeline.IsScenePipeline() == TRUE)
                        {
                            UINT vertexInputResourceCounter = 0;

                            const UINT camNum = pScene->GetSceneCameraNum();
                            for (UINT camIndex = 0; camIndex < camNum; ++camIndex)
                            {
                                Object::Camera::CameraBase* pCam = pScene->GetCamera(camIndex);
                                if (pCam->IsEnable() == TRUE)
                                {
                                    const UINT modelNum = pScene->GetSceneModelNum();

                                    for (UINT modelIndex = 0; modelIndex < modelNum; ++modelIndex)
                                    {
                                        Object::Model::ModelObject* pModel = pScene->GetModel(modelIndex);

                                        if (pModel->IsEnable() == TRUE)
                                        {
                                            Buffer::VulkanDescriptorBuffer* pDescriptorBuffer = NULL;
                                            UINT dynamicUniformBufferOffset                   = 0;

                                            if (uniformBufferDescUpdateInfoList.size() > 0)
                                            {
                                                pDescriptorBuffer = uniformBufferDescUpdateInfoList[0].pDescriptorBuffer;
                                                dynamicUniformBufferOffset =
                                                    (camIndex * modelIndex + modelIndex) *
                                                    static_cast<UINT>(pDescriptorBuffer->GetDescriptorObjectSize());
                                            }

                                            const UINT meshNum = pModel->GetMeshNum();
                                            for (UINT meshIndex = 0; meshIndex < meshNum; ++meshIndex)
                                            {
                                                const VulkanVertexInputResource* pVertexInputResource =
                                                    &(pVertexInputResourceList->at(vertexInputResourceCounter));

                                                Buffer::VulkanVertexBuffer* pVertexBuffer = pVertexInputResource->pVertexBuffer.get();
                                                Buffer::VulkanIndexBuffer*  pIndexBuffer  = pVertexInputResource->pIndexBuffer.get();

                                                pCmdBuffer->cmdBindVertexBuffers({ pVertexBuffer->GetVertexBuffer() },
                                                                                 { 0 });

                                                pCmdBuffer->cmdBindIndexBuffers(pIndexBuffer->GetBuffer(),
                                                                                { 0 },
                                                                                pIndexBuffer->GetIndexType());

                                                if (uniformBufferDescUpdateInfoList.size()  > 0 &&
                                                    m_pDescriptorMgr->GetDescriptorSetNum() > 0)
                                                {
                                                    if (m_pDescriptorMgr->GetDescriptorSet(subpassIndex) != VK_NULL_HANDLE)
                                                    {
                                                        pCmdBuffer->cmdBindDynamicDescriptorSets(
                                                            graphicsPipeline.GetGraphicsPipelineLayout(),
                                                            { m_pDescriptorMgr->GetDescriptorSet(subpassIndex) },
                                                            { dynamicUniformBufferOffset });
                                                    }
                                                }

                                                pCmdBuffer->cmdDrawElements(graphicsPipeline.GetGraphicsPipelineHandle(),
                                                                            pIndexBuffer->GetIndexNum(),
                                                                            0,
                                                                            0);

                                                assert(status == BX_SUCCESS);

                                                vertexInputResourceCounter++;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            for (UINT vertexInputIndex = 0; vertexInputIndex < vertexInputResourceNum; ++vertexInputIndex)
                            {
                                const VulkanVertexInputResource* pVertexInputResource =
                                    &(pVertexInputResourceList->at(vertexInputIndex));

                                Buffer::VulkanVertexBuffer* pVertexBuffer = pVertexInputResource->pVertexBuffer.get();
                                Buffer::VulkanIndexBuffer*  pIndexBuffer  = pVertexInputResource->pIndexBuffer.get();

                                pCmdBuffer->cmdBindVertexBuffers({ pVertexBuffer->GetVertexBuffer() },
                                                                 { 0 });

                                pCmdBuffer->cmdBindIndexBuffers(pIndexBuffer->GetBuffer(),
                                                                { 0 },
                                                                pIndexBuffer->GetIndexType());

                                pCmdBuffer->cmdBindDescriptorSets(graphicsPipeline.GetGraphicsPipelineLayout(),
                                                                  { m_pDescriptorMgr->GetDescriptorSet(subpassIndex) });

                                pCmdBuffer->cmdDrawElements(graphicsPipeline.GetGraphicsPipelineHandle(),
                                                            pIndexBuffer->GetIndexNum(),
                                                            0,
                                                            0);

                                assert(status == BX_SUCCESS);
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
        }

        BOOL VulkanRenderPass::createRenderTargets(
            IN  const std::vector<VulkanRenderTargetCreateData*>*    pRenderTargetsCreateDataRefList,
            IN  const std::vector<const VulkanDescriptorInfo*>*      pSubpassInputAttachmentDescriptorInfoPtrList,
            IN  const std::vector<const VulkanDescriptorInfo*>*      pSubpassResolveAttachmentDescriptorInfoPtrList,
            OUT VkSubpassDescription*                                pSubpassDescription,
            OUT std::vector<VkAttachmentDescription>*                pAttachmentDescriptionList,
            OUT std::vector<VkAttachmentReference>*                  pColorSubpassAttachmentRefList,
            OUT VkAttachmentReference*                               pDepthSubpassAttachmentRef,
            OUT std::vector<VkAttachmentReference>*                  pInputSubpassAttachmentRef,
            OUT std::vector<VkAttachmentReference>*                  pResolveSubpassAttachmentRef,
            OUT std::vector<std::vector<Buffer::VulkanAttachment*>>* pFramebuffersAttachmentsTable)
        {
            BOOL status = BX_SUCCESS;

            size_t renderTargetNum = pRenderTargetsCreateDataRefList->size();

            /// Create render pass
            for (size_t i = 0; i < renderTargetNum; ++i)
            {
                VulkanRenderTargetCreateData* pRenderTargetCreateData = pRenderTargetsCreateDataRefList->at(i);

                VkFormat attachmentFormat = VK_FORMAT_UNDEFINED;

                for (const VulkanRenderTargetFramebufferCreateData& renderTargetFramebufferCreateData :
                    *(pRenderTargetCreateData->pRenderTargetFramebufferCreateData))
                {
                    const UINT framebufferIndex                 = renderTargetFramebufferCreateData.framebufferIndex;
                    const Buffer::VulkanAttachment* pAttachment = &(renderTargetFramebufferCreateData.attachment);

                    pFramebuffersAttachmentsTable->at(framebufferIndex).at(pRenderTargetCreateData->attachmentIndex) =
                        const_cast<Buffer::VulkanAttachment*>(pAttachment);

                    const Texture::VulkanTextureBase* pTex = pAttachment->pTex;

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

                VkAttachmentDescription attachmentDescription = {};
                attachmentDescription.format         = attachmentFormat;
                attachmentDescription.samples        =
                    Utility::VulkanUtility::GetVkSampleCount(pRenderTargetCreateData->sampleNum);
                attachmentDescription.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachmentDescription.storeOp        = ((pRenderTargetCreateData->isStore == TRUE) ?
                                                              VK_ATTACHMENT_STORE_OP_STORE :
                                                              VK_ATTACHMENT_STORE_OP_DONT_CARE);

                if (pRenderTargetCreateData->useStencil == TRUE)
                {
                    attachmentDescription.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                    attachmentDescription.stencilStoreOp = ((pRenderTargetCreateData->isStoreStencil) ?
                                                                  VK_ATTACHMENT_STORE_OP_STORE :
                                                                  VK_ATTACHMENT_STORE_OP_DONT_CARE);
                }
                else
                {
                    attachmentDescription.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                    attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                }

                attachmentDescription.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
                attachmentDescription.finalLayout    =
                    Utility::VulkanUtility::GetAttachmentVkImageLayout(pRenderTargetCreateData->layout);

                pAttachmentDescriptionList->at(pRenderTargetCreateData->attachmentIndex) = attachmentDescription;

                VkAttachmentReference attachmentRef = {};
                attachmentRef.attachment            = pRenderTargetCreateData->attachmentIndex;
                attachmentRef.layout                =
                    Utility::VulkanUtility::GetAttachmentRefVkImageLayout(pRenderTargetCreateData->layout);

                if (m_pSetting->m_graphicsSetting.antialasing == AA_NONE ||
                    pRenderTargetCreateData->layout           != BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_PRESENT)
                {
                    switch (pRenderTargetCreateData->layout)
                    {
                        case BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_COLOR:
                        case BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_PRESENT:
                            pColorSubpassAttachmentRefList->push_back(attachmentRef);
                            break;
                        case BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_DEPTH_STENCIL:
                            *pDepthSubpassAttachmentRef = attachmentRef;
                            break;
                        default:
                            NotSupported();
                            break;
                    }
                }
            }

            pSubpassDescription->pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
            pSubpassDescription->colorAttachmentCount = static_cast<UINT>(pColorSubpassAttachmentRefList->size());
            pSubpassDescription->pColorAttachments    = pColorSubpassAttachmentRefList->data();

            if (IsDepthEnabled()                   == TRUE &&
                pDepthSubpassAttachmentRef->layout != VK_IMAGE_LAYOUT_UNDEFINED)
            {
                pSubpassDescription->pDepthStencilAttachment = pDepthSubpassAttachmentRef;
            }

            const size_t subpassInputAttchmentDescriptorInfoSize = pSubpassInputAttachmentDescriptorInfoPtrList->size();
            for (size_t subpassAttachmentDescriptorInfoIndex = 0;
                 subpassAttachmentDescriptorInfoIndex < subpassInputAttchmentDescriptorInfoSize;
                 ++subpassAttachmentDescriptorInfoIndex)
            {
                pInputSubpassAttachmentRef->
                    push_back({ pSubpassInputAttachmentDescriptorInfoPtrList->at(subpassAttachmentDescriptorInfoIndex)->attachmentIndex,
                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL });
            }

            if (subpassInputAttchmentDescriptorInfoSize > 0)
            {
                pSubpassDescription->inputAttachmentCount = static_cast<UINT>(pInputSubpassAttachmentRef->size());
                pSubpassDescription->pInputAttachments    = pInputSubpassAttachmentRef->data();
            }

            const size_t subpassResolveAttachmentDescriptorInfoSize = pSubpassResolveAttachmentDescriptorInfoPtrList->size();
            for (size_t subpassResolveAttachmentDescriptorInfoIndex = 0;
                 subpassResolveAttachmentDescriptorInfoIndex < subpassResolveAttachmentDescriptorInfoSize;
                 ++subpassResolveAttachmentDescriptorInfoIndex)
            {
                pResolveSubpassAttachmentRef->
                    push_back({ pSubpassResolveAttachmentDescriptorInfoPtrList->at(subpassResolveAttachmentDescriptorInfoIndex)->attachmentIndex,
                                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
            }

            if (subpassResolveAttachmentDescriptorInfoSize > 0)
            {
                pSubpassDescription->pResolveAttachments = pResolveSubpassAttachmentRef->data();
            }

            return status;
        }

        BOOL VulkanRenderPass::createRenderPass(
            const UINT                                  renderSubpassNum,
            const std::vector<VkSubpassDescription>&    subpassDescriptionList,
            const std::vector<VkAttachmentDescription>& attachmentDescriptionList,
            const std::vector<VkSubpassDependency>*     pSubpassDependencyList)
        {
            BOOL status = BX_SUCCESS;

            VkRenderPassCreateInfo renderPassCreateInfo = {};
            renderPassCreateInfo.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassCreateInfo.attachmentCount        = static_cast<UINT>(attachmentDescriptionList.size());
            renderPassCreateInfo.pAttachments           = attachmentDescriptionList.data();
            renderPassCreateInfo.subpassCount           = renderSubpassNum;
            renderPassCreateInfo.pSubpasses             = subpassDescriptionList.data();

            if (pSubpassDependencyList != NULL)
            {
                renderPassCreateInfo.dependencyCount = static_cast<UINT>(pSubpassDependencyList->size());
                renderPassCreateInfo.pDependencies   = pSubpassDependencyList->data();
            }

            VkResult createRenderpassResult =
                vkCreateRenderPass(*m_pDevice, &renderPassCreateInfo, NULL, m_renderPass.replace());
            status = ((createRenderpassResult == VK_SUCCESS) ? BX_SUCCESS : BX_FAIL);

            assert(status == BX_SUCCESS);

            return status;
        }

        BOOL VulkanRenderPass::createFramebuffers(
            std::vector<std::vector<Buffer::VulkanAttachment*>>* pFramebufferAttachmentPtrList,
            const UINT                                           framebufferNum)
        {
            BOOL status = BX_SUCCESS;

            // Generate framebuffers
            m_framebufferList.resize(framebufferNum, Buffer::VulkanFramebuffer(m_pDevice));

            if (status == BX_SUCCESS)
            {
                for (UINT i = 0; i < framebufferNum; ++i)
                {
                    const std::vector<Buffer::VulkanAttachment*>& attachmentPtrList = pFramebufferAttachmentPtrList->at(i);

                    const UINT framebufferWidth  = attachmentPtrList[0]->pTex->GetTextureWidth();
                    const UINT framebufferHeight = attachmentPtrList[0]->pTex->GetTextureHeight();
                    const UINT framebufferLayers = attachmentPtrList[0]->pTex->GetTextureLayers();

                    Buffer::FramebufferCreateData framebufferCreateData = {};
                    framebufferCreateData.renderPass                    = m_renderPass;
                    framebufferCreateData.ppAttachments                 = &(pFramebufferAttachmentPtrList->at(i));
                    framebufferCreateData.width                         = framebufferWidth;
                    framebufferCreateData.height                        = framebufferHeight;
                    framebufferCreateData.layers                        = framebufferLayers;

                    m_framebufferList[i].createFramebuffer(framebufferCreateData);
                }
            }

            return status;
        }
    }
}
