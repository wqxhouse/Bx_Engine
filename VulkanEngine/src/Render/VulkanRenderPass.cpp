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

            std::vector<std::vector<VkAttachmentReference>> colorSubpassAttachmentRefList(renderSubpassNum);
            std::vector<VkAttachmentReference>              depthSubpassAttachmentRefList(renderSubpassNum);
            std::vector<std::vector<VkAttachmentReference>> inputSubpassAttachmentRefList(renderSubpassNum);

            std::vector<std::vector<Texture::VulkanTextureBase*>> framebuffersTextureTable(renderPassCreateData.framebufferNum);

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

            for (UINT i = 0; i < renderSubpassNum; ++i)
            {
                const VulkanRenderSubpassCreateData& subpassCreateData = pRenderSubpassCreateDataList->at(i);

                VulkanSubpassGraphicsPipelineCreateData*
                    pSubpassGraphicsPipelineCreateData = subpassCreateData.pSubpassGraphicsPipelineCreateData;

                const UINT subpassIndex                = pSubpassGraphicsPipelineCreateData->subpassIndex;

                status = createRenderTargets(subpassCreateData.pSubpassRenderTargetCreateDataRefList,
                                             &(subPassDescriptionList[subpassIndex]),
                                             &attachmentDescriptionList,
                                             &(colorSubpassAttachmentRefList[subpassIndex]),
                                             &(depthSubpassAttachmentRefList[subpassIndex]),
                                             &(inputSubpassAttachmentRefList[subpassIndex]),
                                             &(framebuffersTextureTable));

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

            status = createFramebuffers(&(framebuffersTextureTable),
                                        renderPassCreateData.framebufferNum);

            // TODO: Support the multipass in a single descriptor pool
            // Create descriptor pool
            std::vector<Mgr::DescriptorPoolCreateInfo> descriptorPoolCreateDataList;

            std::vector<VulkanDescriptorResources*> pDescriptorResourcesRefList(renderSubpassNum);

            size_t uniformBufferDescriptorNum   = 0;
            size_t textureDescriptorNum         = 0;
            size_t inputAttachmentDescriptorNum = 0;

            // Iterate all subpasses (no necessary follow the subpass index order)
            for (UINT subpassIter = 0; subpassIter < renderSubpassNum; ++subpassIter)
            {
                const VulkanRenderSubpassCreateData& subpassCreateData =
                    pRenderSubpassCreateDataList->at(subpassIter);

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
                        inputAttachmentDescriptorNum += descriptorResources->pInputAttachmentList->size();
                    }
                }
            }

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

                VulkanGraphicsPipelineCreateData graphicsPipelineCreateData = {};
                graphicsPipelineCreateData.subpassIndex                     = subpassIndex;
                graphicsPipelineCreateData.enableColor                      = IsColorEnabled();
                graphicsPipelineCreateData.enableDepth                      = IsDepthEnabled();
                graphicsPipelineCreateData.enableStencil                    = IsStencilEnabled();
                graphicsPipelineCreateData.pProps                           = pSubpassGraphicsPipelineCreateData->pProps;
                graphicsPipelineCreateData.pShaderMeta                      = pSubpassGraphicsPipelineCreateData->pShaderMeta;
                graphicsPipelineCreateData.pResource                        = pSubpassGraphicsPipelineCreateData->pResource;
                graphicsPipelineCreateData.isScenePipeline                  = pSubpassGraphicsPipelineCreateData->isScenePipeline;

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
            IN  const std::vector<VulkanRenderTargetCreateData*>*      pRenderTargetsCreateDataRefList,
            OUT VkSubpassDescription*                                  pSubpassDescription,
            OUT std::vector<VkAttachmentDescription>*                  pAttachmentDescriptionList,
            OUT std::vector<VkAttachmentReference>*                    pColorSubpassAttachmentRefList,
            OUT VkAttachmentReference*                                 pDepthSubpassAttachmentRef,
            OUT std::vector<VkAttachmentReference>*                    pInputSubpassAttachmentRef,
            OUT std::vector<std::vector<Texture::VulkanTextureBase*>>* pFramebuffersTextureTable)
        {
            BOOL status = BX_SUCCESS;

            size_t renderTargetNum = pRenderTargetsCreateDataRefList->size();

            /// Create render pass
            for (size_t i = 0; i < renderTargetNum; ++i)
            {
                VulkanRenderTargetCreateData* pRenderPassCreateData = pRenderTargetsCreateDataRefList->at(i);

                VkFormat attachmentFormat = VK_FORMAT_UNDEFINED;

                for (const VulkanRenderTargetFramebufferCreateData& renderTargetFramebufferCreateData :
                    *(pRenderPassCreateData->pRenderTargetFramebufferCreateData))
                {
                    UINT framebufferIndex            = renderTargetFramebufferCreateData.framebufferIndex;
                    Texture::VulkanTextureBase* pTex = renderTargetFramebufferCreateData.pTexture;

                    pFramebuffersTextureTable->at(framebufferIndex).push_back(pTex);

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
                    Utility::VulkanUtility::GetVkSampleCount(m_pSetting->m_graphicsSetting.antialasing);
                attachmentDescription.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachmentDescription.storeOp        = ((pRenderPassCreateData->isStore == TRUE) ?
                                                              VK_ATTACHMENT_STORE_OP_STORE :
                                                              VK_ATTACHMENT_STORE_OP_DONT_CARE);

                if (pRenderPassCreateData->useStencil == TRUE)
                {
                    attachmentDescription.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                    attachmentDescription.stencilStoreOp = ((pRenderPassCreateData->isStoreStencil) ?
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
                    Utility::VulkanUtility::GetAttachmentVkImageLayout(pRenderPassCreateData->layout);

                pAttachmentDescriptionList->push_back(attachmentDescription);

                VkAttachmentReference attachmentRef = {};
                attachmentRef.attachment            = pRenderPassCreateData->bindingPoint;// Output layout index in shader,
                                                                                          // e.g. layout(location = 0) out vec4 outColor
                attachmentRef.layout                =
                    Utility::VulkanUtility::GetAttachmentRefVkImageLayout(pRenderPassCreateData->layout);

                switch (pRenderPassCreateData->layout)
                {
                    case BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_COLOR:
                    case BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_PRESENT:
                        pColorSubpassAttachmentRefList->push_back(attachmentRef);
                        break;
                    case BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_DEPTH_STENCIL:
                        *pDepthSubpassAttachmentRef = attachmentRef;
                        break;
                    case BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_ATTACHMENT_INPUT:
                        pInputSubpassAttachmentRef->push_back(attachmentRef);
                        break;
                    default:
                        NotSupported();
                        break;
                }
            }

            pSubpassDescription->pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
            pSubpassDescription->colorAttachmentCount = static_cast<UINT>(pColorSubpassAttachmentRefList->size());
            pSubpassDescription->pColorAttachments    = pColorSubpassAttachmentRefList->data();

            if (IsDepthEnabled()           == TRUE &&
                pDepthSubpassAttachmentRef != NULL)
            {
                pSubpassDescription->pDepthStencilAttachment = pDepthSubpassAttachmentRef;
            }

            if (pInputSubpassAttachmentRef != NULL)
            {
                pSubpassDescription->inputAttachmentCount = static_cast<UINT>(pInputSubpassAttachmentRef->size());
                pSubpassDescription->pInputAttachments    = pInputSubpassAttachmentRef->data();
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
            std::vector<std::vector<Texture::VulkanTextureBase*>>* pFramebuffersTexturePtrList,
            const UINT                                             framebufferNum)
        {
            BOOL status = BX_SUCCESS;

            // Generate framebuffers
            m_framebufferList.resize(framebufferNum, Buffer::VulkanFramebuffer(m_pDevice));

            if (status == BX_SUCCESS)
            {
                for (UINT i = 0; i < framebufferNum; ++i)
                {
                    const std::vector<Texture::VulkanTextureBase*>& texturePtrList = pFramebuffersTexturePtrList->at(i);

                    UINT framebufferWidth  = texturePtrList[0]->GetTextureWidth();
                    UINT framebufferHeight = texturePtrList[0]->GetTextureHeight();
                    UINT framebufferLayers = texturePtrList[0]->GetTextureLayers();

                    Buffer::FramebufferCreateData framebufferCreateData = {};
                    framebufferCreateData.renderPass                    = m_renderPass;
                    framebufferCreateData.ppAttachments                 = &(pFramebuffersTexturePtrList->at(i));
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