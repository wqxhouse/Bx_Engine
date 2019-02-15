//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "VulkanRender.h"

namespace VulkanEngine
{
    namespace Render
    {
        VulkanForwardRender::VulkanForwardRender(
            const Setting*                                const pSetting,
            const VkPhysicalDevice*                       const pHwDevice,
            const VkDevice*                               const pDevice,
            Mgr::CmdBufferMgr*                            const pCmdBufferMgr,
            Mgr::DescriptorMgr*                           const pDescritorMgr,
            Mgr::TextureMgr*                              const pTextureMgr,
            const Scene::RenderScene*                           pScene,
            const std::vector<Texture::VulkanTexture2D*>* const ppBackbufferTextures)
            : VulkanRenderBase(pSetting, pHwDevice, pDevice, pCmdBufferMgr, pDescritorMgr, pTextureMgr,
                               pScene, ppBackbufferTextures)
        {
        }

        VulkanForwardRender::~VulkanForwardRender()
        {
        }

        BOOL VulkanForwardRender::initialize()
        {
            BOOL status = BX_SUCCESS;

            /// Initialize all render passes which need to be used in forward render

            /// Initialize default render pass for main scene
            //  Initialize graphics pipeline properties
            VulkanRenderProperties props = {};
            props.enableBlending         = TRUE;
            props.sceneClearValue        = { { 0.0f, 0.0f, 0.0f, 1.0f } };
            props.renderViewportRect     =
            {
                0.0f, static_cast<float>(m_pSetting->resolution.width),
                static_cast<float>(m_pSetting->resolution.height), 0.0f
            };
            props.enableColor            = TRUE;

            // Initialize shaders
            Shader::BxShaderMeta mainSceneShaderMeta          = {};
            mainSceneShaderMeta.vertexShaderInfo.shaderFile   = "MainSceneForward.vert.spv";
            mainSceneShaderMeta.fragmentShaderInfo.shaderFile = "MainSceneForward.frag.spv";

            /// Initialize render pass
            std::vector<VulkanRenderTargetCreateDescriptor> renderTargetDescriptors =
            {
                { TRUE, 0, 0, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_PRESENT, FALSE, FALSE }
            };

            assert((IsDepthTestEnabled() == TRUE)  ||
                   ((IsDepthTestEnabled()   == FALSE) &&
                    (IsStencilTestEnabled() == FALSE)));

            // Create depth buffer
            if (IsDepthTestEnabled() == TRUE)
            {
                props.enableDepth     = TRUE;
                props.depthClearValue = { 1.0f, 0.0f };

                genBackbufferDepthBuffer();

                renderTargetDescriptors.push_back({ TRUE, 0, 1, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_DEPTH_STENCIL, FALSE, FALSE });
            }

            if (IsStencilTestEnabled() == TRUE)
            {
                props.enableStencil     = TRUE;
                props.stencilClearValue = { 1.0f, 0.0f };
            }

            // Generate render pass create data
            const UINT backbufferAttachmentNum = static_cast<UINT>(m_backBufferRTsCreateDataList[0].size());
            const UINT backbufferNum           = static_cast<const UINT>(m_backBufferRTsCreateDataList.size());

            std::vector<VulkanRenderTargetCreateData>                         renderTargetsCreateData(backbufferAttachmentNum);
            std::vector<std::vector<VulkanRenderTargetFramebufferCreateData>> renderTargetsFramebuffersCreateData(backbufferAttachmentNum);

            for (UINT i = 0; i < backbufferAttachmentNum; ++i)
            {
                VulkanRenderTargetCreateData* pRenderTargetCreateData = &(renderTargetsCreateData[i]);

                pRenderTargetCreateData->renderSubPassIndex = renderTargetDescriptors[i].renderSubPassIndex;
                pRenderTargetCreateData->bindingPoint       = renderTargetDescriptors[i].bindingPoint;
                pRenderTargetCreateData->isStore            = renderTargetDescriptors[i].isStore;
                pRenderTargetCreateData->layout             = renderTargetDescriptors[i].layout;
                pRenderTargetCreateData->useStencil         = renderTargetDescriptors[i].useStencil;
                pRenderTargetCreateData->isStoreStencil     = renderTargetDescriptors[i].isStoreStencil;

                std::vector<VulkanRenderTargetFramebufferCreateData>*
                    pRenderTargetsFramebufferCreateData = &(renderTargetsFramebuffersCreateData[i]);

                pRenderTargetsFramebufferCreateData->resize(backbufferNum);
                for (UINT j = 0; j < backbufferNum; ++j)
                {
                    pRenderTargetsFramebufferCreateData->at(j) = m_backBufferRTsCreateDataList[j][i];
                }

                pRenderTargetCreateData->pRenderTargetFramebufferCreateData = pRenderTargetsFramebufferCreateData;
            }

            // Initialize render pass resources
            // std::vector<VulkanUniformBufferResource*> uniformbufferResources;

            // Initialize vertex input for render pass
            VulkanRenderResources renderSources         = {};
            renderSources.vertexBufferTexChannelNum     = 1;
            renderSources.vertexDescriptionBindingPoint = 0;
            renderSources.pVertexInputResourceList      = &m_mainSceneVertexInputResourceList;

            // Initialize uniform buffers for render pass
            std::vector<VulkanUniformBufferResource> uniformBufferResourceList = createTransUniformBufferResource();
            renderSources.pUniformBufferResourceList                           = &uniformBufferResourceList;

            // Initialize textures for render pass
            std::vector<VulkanTextureResource> sceneTextureResourceList = createSceneTextures();
            renderSources.pTextureResouceList                           = &sceneTextureResourceList;

            // Create render pass create data
            VulkanGraphicsPipelineProperties subpassGraphicsPipelineProperties = {};
            subpassGraphicsPipelineProperties.cullMode                         = CULLMODE_BACK;
            subpassGraphicsPipelineProperties.polyMode                         = PolyMode::POLYMODE_FILL;
            subpassGraphicsPipelineProperties.viewportRects                    = { props.renderViewportRect };
            subpassGraphicsPipelineProperties.scissorRects                     = { props.renderViewportRect };

            VulkanSubpassGraphicsPipelineCreateData subpassGraphicsPipelineCreateData = {};
            subpassGraphicsPipelineCreateData.subpassIndex                            = 0;
            subpassGraphicsPipelineCreateData.pProps                                  = &subpassGraphicsPipelineProperties;
            subpassGraphicsPipelineCreateData.pShaderMeta                             = &mainSceneShaderMeta;
            subpassGraphicsPipelineCreateData.pResource                               = &renderSources;

            std::vector<VulkanRenderSubpassCreateData> renderSubpassCreateDataList(1);
            renderSubpassCreateDataList[0].pSubpassGraphicsPipelineCreateData = &subpassGraphicsPipelineCreateData;
            renderSubpassCreateDataList[0].pRenderTargetCreateDataList        = &renderTargetsCreateData;

            VulkanRenderpassCreateData renderPassCreateData = {};
            renderPassCreateData.pRenderProperties          = &props;
            renderPassCreateData.pSubpassCreateDataList     = &renderSubpassCreateDataList;
            renderPassCreateData.framebufferNum             = backbufferNum;

            status = m_mainSceneRenderPass.create(renderPassCreateData);

            assert(status == BX_SUCCESS);

            return status;
        }

        BOOL VulkanForwardRender::update(
            const float deltaTime)
        {
            BOOL status = BX_SUCCESS;

            for (VulkanRenderPass preRenderPass : m_preDrawPassList)
            {
                // status = preRenderPass.update(deltaTime);

                assert(status == BX_SUCCESS);
            }

            const Scene::RenderScene* pScene = m_pScene;

            const UINT camNum   = pScene->GetSceneCameraNum();
            const UINT modelNum = pScene->GetSceneModelNum();

            for (UINT camIndex = 0; camIndex < camNum; ++camIndex)
            {
                Object::Camera::CameraBase* pCam = pScene->GetCamera(camIndex);

                if (pCam->IsEnable() == TRUE)
                {
                    pCam->update(deltaTime);

                    const Math::Mat4* pViewMat = &(pCam->GetViewMatrix());
                    const Math::Mat4* pProspectMat = &(pCam->GetProjectionMatrix());

                    const Math::Mat4 vpMat = (*pProspectMat) * (*pViewMat);

                    for (UINT modelIndex = 0; modelIndex < modelNum; ++modelIndex)
                    {
                        Object::Model::ModelObject* pModel = pScene->GetModel(modelIndex);

                        if (pModel->IsEnable() == TRUE)
                        {
                            m_transUniformbuffer[modelIndex].worldMat = pModel->GetTrans()->GetTransMatrix();
                            m_transUniformbuffer[modelIndex].viewMat  = *pViewMat;
                            m_transUniformbuffer[modelIndex].projMat  = *pProspectMat;
                            m_transUniformbuffer[modelIndex].wvpMat   = vpMat * m_transUniformbuffer[modelIndex].worldMat;
                        }
                    }
                }
            }

            status = m_mainSceneRenderPass.update(deltaTime, m_descriptorUpdateDataList);
            assert(status == BX_SUCCESS);

            for (VulkanRenderPass postRenderPass : m_postDrawPassList)
            {
                // status = postRenderPass.update(deltaTime);

                assert(status == BX_SUCCESS);
            }

            return status;
        }

        void VulkanForwardRender::draw()
        {
            BOOL status = BX_SUCCESS;

            for (VulkanRenderPass preRenderPass : m_preDrawPassList)
            {
                status = preRenderPass.draw();

                assert(status == BX_SUCCESS);
            }

            m_mainSceneRenderPass.draw();

            assert(status == BX_SUCCESS);

            for (VulkanRenderPass postRenderPass : m_postDrawPassList)
            {
                status = postRenderPass.draw();

                assert(status == BX_SUCCESS);
            }
        }
    }
}