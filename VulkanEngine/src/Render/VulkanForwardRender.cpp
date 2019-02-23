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

            std::vector<VulkanRenderTargetCreateData>                         renderTargetsCreateDataList(backbufferAttachmentNum);
            std::vector<std::vector<VulkanRenderTargetFramebufferCreateData>> renderTargetsFramebuffersCreateData(backbufferAttachmentNum);

            for (UINT attachmentIndex = 0; attachmentIndex < backbufferAttachmentNum; ++attachmentIndex)
            {
                VulkanRenderTargetCreateData* pRenderTargetCreateData = &(renderTargetsCreateDataList[attachmentIndex]);

                pRenderTargetCreateData->renderSubPassIndex = renderTargetDescriptors[attachmentIndex].renderSubPassIndex;
                pRenderTargetCreateData->bindingPoint       = renderTargetDescriptors[attachmentIndex].bindingPoint;
                pRenderTargetCreateData->attachmentIndex    = attachmentIndex;
                pRenderTargetCreateData->isStore            = renderTargetDescriptors[attachmentIndex].isStore;
                pRenderTargetCreateData->layout             = renderTargetDescriptors[attachmentIndex].layout;
                pRenderTargetCreateData->useStencil         = renderTargetDescriptors[attachmentIndex].useStencil;
                pRenderTargetCreateData->isStoreStencil     = renderTargetDescriptors[attachmentIndex].isStoreStencil;

                std::vector<VulkanRenderTargetFramebufferCreateData>*
                    pRenderTargetsFramebufferCreateData = &(renderTargetsFramebuffersCreateData[attachmentIndex]);

                pRenderTargetsFramebufferCreateData->resize(backbufferNum);
                for (UINT framebufferIndex = 0; framebufferIndex < backbufferNum; ++framebufferIndex)
                {
                    pRenderTargetsFramebufferCreateData->at(framebufferIndex) =
                        m_backBufferRTsCreateDataList[framebufferIndex][attachmentIndex];
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

            std::vector<VulkanDescriptorResources> descriptorResourceList;

            VulkanDescriptorResources descriptorResources = {};
            descriptorResources.descriptorSetIndex        = 0;

            // Initialize uniform buffers for render pass
            std::vector<VulkanUniformBufferResource> uniformBufferResourceList = createTransUniformBufferResource();
            descriptorResources.pUniformBufferResourceList                     = &uniformBufferResourceList;

            // Initialize textures for render pass
            ::Texture::TextureSamplerCreateData textureSamplerCreateData = {};
            textureSamplerCreateData.minFilter                           = BX_TEXTURE_SAMPLER_FILTER_LINEAR;
            textureSamplerCreateData.magFilter                           = BX_TEXTURE_SAMPLER_FILTER_LINEAR;
            textureSamplerCreateData.addressingModeU                     = BX_TEXTURE_SAMPLER_ADDRESSING_CLAMP_TO_EDGE;
            textureSamplerCreateData.addressingModeV                     = BX_TEXTURE_SAMPLER_ADDRESSING_CLAMP_TO_EDGE;
            textureSamplerCreateData.anisotropyNum                       = static_cast<float>(m_pSetting->m_graphicsSetting.anisotropy);
            textureSamplerCreateData.borderColor                         = { 0.0f, 0.0f, 0.0f, 1.0f };
            textureSamplerCreateData.normalize                           = TRUE;
            textureSamplerCreateData.mipmapFilter                        = BX_TEXTURE_SAMPLER_FILTER_LINEAR;
            textureSamplerCreateData.mipmapOffset                        = 0.0f;
            textureSamplerCreateData.minLod                              = 0.0f;
            textureSamplerCreateData.maxLod                              = 0.0f;

            Texture::VulkanTexture2D* pTexture =
                m_pTextureMgr->createTexture2DSampler("../resources/textures/teaport/wall.jpg",
                    m_pSetting->m_graphicsSetting.antialasing,
                    FALSE,
                    BX_FORMAT_RGBA8,
                    BX_FORMAT_RGBA8,
                    textureSamplerCreateData);

            std::vector<VulkanTextureResource> sceneTextureResourceList = { createSceneTextures(0, 1, 1, pTexture) };
            descriptorResources.pTextureResouceList                     = &sceneTextureResourceList;

            // Added descriptor resources to the resource list
            descriptorResourceList.push_back(descriptorResources);

            renderSources.pDescriptorResourceList = &descriptorResourceList;

            // Create render pass create data
            std::vector<VulkanGraphicsPipelineRenderTargetProperties> renderTargetsProps(1);
            renderTargetsProps[0].enableBlend = m_pSetting->m_graphicsSetting.blend;

            VulkanGraphicsPipelineProperties subpassGraphicsPipelineProperties = {};
            subpassGraphicsPipelineProperties.cullMode                         = CULLMODE_BACK;
            subpassGraphicsPipelineProperties.polyMode                         = PolyMode::POLYMODE_FILL;
            subpassGraphicsPipelineProperties.viewportRects                    = { props.renderViewportRect };
            subpassGraphicsPipelineProperties.scissorRects                     = { props.renderViewportRect };
            subpassGraphicsPipelineProperties.pRenderTargetsProps              = &renderTargetsProps;

            VulkanSubpassGraphicsPipelineCreateData subpassGraphicsPipelineCreateData = {};
            subpassGraphicsPipelineCreateData.subpassIndex                            = 0;
            subpassGraphicsPipelineCreateData.pProps                                  = &subpassGraphicsPipelineProperties;
            subpassGraphicsPipelineCreateData.pShaderMeta                             = &mainSceneShaderMeta;
            subpassGraphicsPipelineCreateData.pResource                               = &renderSources;
            subpassGraphicsPipelineCreateData.isScenePipeline                         = TRUE;

            std::vector<VulkanRenderTargetCreateData*> renderTargetCreateDataRefList(backbufferAttachmentNum);
            for (UINT attachmentIndex = 0; attachmentIndex < backbufferAttachmentNum; ++attachmentIndex)
            {
                renderTargetCreateDataRefList[attachmentIndex] = &renderTargetsCreateDataList[attachmentIndex];
            }

            std::vector<VulkanRenderSubpassCreateData> renderSubpassCreateDataList(1);
            renderSubpassCreateDataList[0].pSubpassGraphicsPipelineCreateData    = &subpassGraphicsPipelineCreateData;
            renderSubpassCreateDataList[0].pSubpassRenderTargetCreateDataRefList = &renderTargetCreateDataRefList;

            VulkanRenderpassCreateData renderPassCreateData  = {};
            renderPassCreateData.pRenderProperties           = &props;
            renderPassCreateData.pSubpassCreateDataList      = &renderSubpassCreateDataList;
            renderPassCreateData.framebufferNum              = backbufferNum;
            renderPassCreateData.pRenderTargetCreateDataList = &renderTargetsCreateDataList;

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

            status = m_mainSceneRenderPass.update(deltaTime, { m_descriptorUpdateDataList });
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