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
              //m_shader(*pDevice)
        {
            m_renderPass             = { *m_pDevice, vkDestroyRenderPass     };
            /*m_graphicsPipeline       = { *m_pDevice, vkDestroyPipeline       };
            m_graphicsPipelineLayout = { *m_pDevice, vkDestroyPipelineLayout };*/
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
                for (const VulkanGraphicsPipelineCreateData& graphicsPipelineCreateData :
                     *(renderPassCreateData.pGraphicsPipelineCreateDataList))
                {
                    status = createGraphicsPipeline(graphicsPipelineCreateData);
                }

                assert(status == BX_SUCCESS);
            }

            return status;
        }

        BOOL VulkanRenderPass::update(
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

                    if (m_enableColor == TRUE)
                    {
                        clearColorValueList.push_back(m_clearColor);
                    }

                    if (m_enableDepth == TRUE)
                    {
                        clearColorValueList.push_back(m_depthColor);
                    }

                    if (m_enableStencil == TRUE)
                    {
                        clearColorValueList.push_back(m_stencilColor);
                    }

                    pCmdBuffer->beginRenderPass(m_renderPass,
                                                m_framebufferList[i].GetFramebufferHandle(),
                                                m_renderViewport,
                                                clearColorValueList);

                    const UINT camNum = pScene->GetSceneCameraNum();
                    for (UINT camIndex = 0; camIndex < camNum; ++camIndex)
                    {
                        Object::Camera::CameraBase* pCam = pScene->GetCamera(camIndex);
                        
                        const UINT vertexInputResourceNum = static_cast<UINT>(m_pVertexInputResourceList->size());
                        UINT vertexInputResourceCounter   = 0;

                        if (pCam->IsEnable() == TRUE)
                        {
                            const UINT modelNum = pScene->GetSceneModelNum();

                            for (UINT modelIndex = 0; modelIndex < modelNum; ++modelIndex)
                            {
                                Object::Model::ModelObject* pModel = pScene->GetModel(modelIndex);

                                // pModel->GetTrans()->TransPos(Math::Vector3(0.0f, -delta, 0.0f));

                                if (pModel->IsEnable() == TRUE)
                                {
                                    Buffer::VulkanDescriptorBuffer* pDescriptorBuffer =
                                        m_uniformBufferDescriptorUpdateInfo[0].pDescriptorBuffer;

                                    const UINT dynamicUniformBufferOffset =
                                        (camIndex * modelIndex + modelIndex) * static_cast<UINT>(pDescriptorBuffer->GetDescriptorObjectSize());

                                    const UINT meshNum = pModel->GetMeshNum();
                                    for (UINT meshIndex = 0; meshIndex < meshNum; ++meshIndex)
                                    {
                                        VulkanVertexInputResource* pVertexInputResource = &(m_pVertexInputResourceList->at(vertexInputResourceCounter));

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
            const UINT                                       renderFramebufferNum)
        {
            BOOL status = BX_SUCCESS;

            size_t renderTargetNum = pRenderTargetsCreateDataList->size();

            std::vector<VkAttachmentDescription>            attachmentDescriptionList(renderTargetNum);
            std::vector<VkSubpassDescription>               subPassDescriptionList(renderSubpassNum);
            std::vector<std::vector<VkAttachmentReference>> colorSubpassAttachmentRefList(renderSubpassNum);

            std::vector<BOOL>                               enableDepth(renderSubpassNum);
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

                switch (renderPassCreateData.layout)
                {
                    case BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_COLOR:
                    case BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_PRESENT:
                        colorSubpassAttachmentRefList[renderPassCreateData.renderSubPassIndex].push_back(attachmentRef);
                        break;
                    case BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_DEPTH_STENCIL:
                        enableDepth[renderPassCreateData.renderSubPassIndex]            = TRUE;
                        depthAttachmentRefList[renderPassCreateData.renderSubPassIndex] = attachmentRef;
                        break;
                    default:
                        NotSupported();
                        break;
                }
            }

            for (UINT i = 0; i < renderSubpassNum; ++i)
            {
                subPassDescriptionList[i].pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
                subPassDescriptionList[i].colorAttachmentCount = static_cast<UINT>(colorSubpassAttachmentRefList.size());
                subPassDescriptionList[i].pColorAttachments    = colorSubpassAttachmentRefList[i].data();

                if (enableDepth[i] == TRUE)
                {
                    subPassDescriptionList[i].pDepthStencilAttachment = &(depthAttachmentRefList[i]);
                }
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
    }
}