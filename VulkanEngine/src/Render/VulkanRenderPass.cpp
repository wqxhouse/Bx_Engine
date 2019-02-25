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
            if (IsColorEnabled())
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

                const UINT subpassIndex            = pSubpassGraphicsPipelineCreateData->subpassIndex;

                status = createRenderTargets(subpassCreateData.pRenderTargetCreateDataList,
                                             &(subPassDescriptionList[subpassIndex]),
                                             &attachmentDescriptionList,
                                             &(colorSubpassAttachmentRefList[subpassIndex]),
                                             &(depthSubpassAttachmentRefList[subpassIndex]),
                                             &(framebuffersTextureTable));

                assert(status == BX_SUCCESS);
            }

            status = createRenderPass(renderSubpassNum, subPassDescriptionList, attachmentDescriptionList);

            assert(status == BX_SUCCESS);

            m_graphicsPipelineList.resize(renderSubpassNum,
                                          VulkanGraphicsPipeline(m_pSetting,
                                                                 m_pDevice,
                                                                 &m_renderPass,
                                                                 m_pCmdBufferMgr,
                                                                 m_pDescriptorMgr));

            status = createFramebuffers(&(framebuffersTextureTable),
                                        renderPassCreateData.framebufferNum);

            for (UINT i = 0; i < renderSubpassNum; ++i)
            {
                const VulkanRenderSubpassCreateData& subpassCreateData = pRenderSubpassCreateDataList->at(i);

                VulkanSubpassGraphicsPipelineCreateData*
                    pSubpassGraphicsPipelineCreateData = subpassCreateData.pSubpassGraphicsPipelineCreateData;

                const UINT subpassIndex = pSubpassGraphicsPipelineCreateData->subpassIndex;

                VulkanGraphicsPipelineCreateData graphicsPipelineCreateData = {};
                graphicsPipelineCreateData.renderTargetNum                  = renderPassCreateData.framebufferNum;
                graphicsPipelineCreateData.enableColor                      = IsColorEnabled();
                graphicsPipelineCreateData.enableDepth                      = IsDepthEnabled();
                graphicsPipelineCreateData.enableStencil                    = IsStencilEnabled();
                graphicsPipelineCreateData.pProps                           = pSubpassGraphicsPipelineCreateData->pProps;
                graphicsPipelineCreateData.pShaderMeta                      = pSubpassGraphicsPipelineCreateData->pShaderMeta;
                graphicsPipelineCreateData.pResource                        = pSubpassGraphicsPipelineCreateData->pResource;

                status = m_graphicsPipelineList[subpassIndex].createGraphicsPipeline(&graphicsPipelineCreateData);

                assert(status == BX_SUCCESS);
            }

            return status;
        }

        BOOL VulkanRenderPass::update(
            const float                                    deltaTime,
            const std::vector<VulkanDescriptorUpdateData>& updateDataList)
        {
            BOOL status = BX_SUCCESS;

            size_t graphicsPipelineNum = m_graphicsPipelineList.size();

            for (size_t pipelineIndex = 0; pipelineIndex < graphicsPipelineNum; ++pipelineIndex)
            {
                status = m_graphicsPipelineList[pipelineIndex].update(deltaTime, updateDataList);
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
                    std::vector<VkClearValue> clearColorValueList;

                    pCmdBuffer->beginRenderPass(m_renderPass,
                                                m_framebufferList[i].GetFramebufferHandle(),
                                                m_renderViewport,
                                                m_clearValueList);

                    const UINT camNum = pScene->GetSceneCameraNum();
                    for (UINT camIndex = 0; camIndex < camNum; ++camIndex)
                    {
                        Object::Camera::CameraBase* pCam = pScene->GetCamera(camIndex);

                        const size_t graphicsPipelineNum = m_graphicsPipelineList.size();
                        for (size_t graphicsPipelineIndex = 0; graphicsPipelineIndex < graphicsPipelineNum; ++graphicsPipelineIndex)
                        {
                            const VulkanGraphicsPipeline& graphicsPipeline = m_graphicsPipelineList[graphicsPipelineIndex];
                            const std::vector<VulkanVertexInputResource>* pVertexInputResourceList =
                                graphicsPipeline.GetVertexInputResourceList();

                            const std::vector<Mgr::DescriptorUpdateInfo>& uniformBufferDescUpdateInfoList =
                                graphicsPipeline.GetUniformBufferDescUpdateInfo();
                        
                            const UINT vertexInputResourceNum = static_cast<UINT>(pVertexInputResourceList->size());

                            UINT vertexInputResourceCounter = 0;

                            if (pCam->IsEnable() == TRUE)
                            {
                                const UINT modelNum = pScene->GetSceneModelNum();

                                for (UINT modelIndex = 0; modelIndex < modelNum; ++modelIndex)
                                {
                                    Object::Model::ModelObject* pModel = pScene->GetModel(modelIndex);

                                    if (pModel->IsEnable() == TRUE)
                                    {
                                        Buffer::VulkanDescriptorBuffer* pDescriptorBuffer =
                                            uniformBufferDescUpdateInfoList[0].pDescriptorBuffer;

                                        const UINT dynamicUniformBufferOffset =
                                            (camIndex * modelIndex + modelIndex) * static_cast<UINT>(pDescriptorBuffer->GetDescriptorObjectSize());

                                        const UINT meshNum = pModel->GetMeshNum();
                                        for (UINT meshIndex = 0; meshIndex < meshNum; ++meshIndex)
                                        {
                                            const VulkanVertexInputResource* pVertexInputResource =
                                                &(pVertexInputResourceList->at(vertexInputResourceCounter));

                                            pCmdBuffer->cmdBindVertexBuffers({ pVertexInputResource->pVertexBuffer->GetVertexBuffer() },
                                                                             { 0 });

                                            Buffer::VulkanIndexBuffer* pIndexBuffer = pVertexInputResource->pIndexBuffer.get();

                                            pCmdBuffer->cmdBindIndexBuffers(pIndexBuffer->GetBuffer(),
                                                                            { 0 },
                                                                            pIndexBuffer->GetIndexType());

                                            if (m_pDescriptorMgr->GetDescriptorSetNum() > 0)
                                            {
                                                if (m_pDescriptorMgr->GetDescriptorSet(0) != VK_NULL_HANDLE)
                                                {
                                                    pCmdBuffer->cmdBindDynamicDescriptorSets(graphicsPipeline.GetGraphicsPipelineLayout(),
                                                                                             { m_pDescriptorMgr->GetDescriptorSet(0) },
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
            IN  const std::vector<VulkanRenderTargetCreateData>*       pRenderTargetsCreateDataList,
            OUT VkSubpassDescription*                                  pSubpassDescription,
            OUT std::vector<VkAttachmentDescription>*                  pAttachmentDescriptionList,
            OUT std::vector<VkAttachmentReference>*                    pColorSubpassAttachmentRefList,
            OUT VkAttachmentReference*                                 pDepthSubpassAttachmentRef,
            OUT std::vector<std::vector<Texture::VulkanTextureBase*>>* pFramebuffersTextureTable)
        {
            BOOL status = BX_SUCCESS;

            size_t renderTargetNum = pRenderTargetsCreateDataList->size();

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
                attachmentDescription.storeOp        = ((renderPassCreateData.isStore == TRUE) ?
                                                              VK_ATTACHMENT_STORE_OP_STORE :
                                                              VK_ATTACHMENT_STORE_OP_DONT_CARE);

                if (renderPassCreateData.useStencil == TRUE)
                {
                    attachmentDescription.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                    attachmentDescription.stencilStoreOp = ((renderPassCreateData.isStoreStencil) ?
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
                    Utility::VulkanUtility::GetAttachmentVkImageLayout(renderPassCreateData.layout);

                pAttachmentDescriptionList->push_back(attachmentDescription);

                VkAttachmentReference attachmentRef = {};
                attachmentRef.attachment            = renderPassCreateData.bindingPoint;// Output layout index in shader,
                                                                                        // e.g. layout(location = 0) out vec4 outColor
                attachmentRef.layout                =
                    Utility::VulkanUtility::GetAttachmentRefVkImageLayout(renderPassCreateData.layout);

                switch (renderPassCreateData.layout)
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

            pSubpassDescription->pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
            pSubpassDescription->colorAttachmentCount = static_cast<UINT>(pColorSubpassAttachmentRefList->size());
            pSubpassDescription->pColorAttachments    = pColorSubpassAttachmentRefList->data();

            if (IsDepthEnabled()           == TRUE &&
                pDepthSubpassAttachmentRef != NULL)
            {
                pSubpassDescription->pDepthStencilAttachment = pDepthSubpassAttachmentRef;
            }

            return status;
        }

        BOOL VulkanRenderPass::createRenderPass(
            const UINT                                  renderSubpassNum,
            const std::vector<VkSubpassDescription>&    subpassDescriptionList,
            const std::vector<VkAttachmentDescription>& attachmentDescriptionList)
        {
            BOOL status = BX_SUCCESS;

            VkRenderPassCreateInfo renderPassCreateInfo = {};
            renderPassCreateInfo.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassCreateInfo.attachmentCount        = static_cast<UINT>(attachmentDescriptionList.size());
            renderPassCreateInfo.pAttachments           = attachmentDescriptionList.data();
            renderPassCreateInfo.subpassCount           = renderSubpassNum;
            renderPassCreateInfo.pSubpasses             = subpassDescriptionList.data();

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
