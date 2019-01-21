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
            const Scene::RenderScene*                           pScene,
            const std::vector<Texture::VulkanTexture2D*>* const ppBackbufferTextures)
            : VulkanRenderBase(pSetting, pHwDevice, pDevice, pCmdBufferMgr, pDescritorMgr, pScene, ppBackbufferTextures)
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
            Render::VulkanRenderProperties props = {};
            props.cullMode                       = CULLMODE_FRONT;
            props.enableBlending                 = TRUE;
            props.polyMode                       = PolyMode::POLYMODE_FILL;
            props.sceneClearValue                = { 0.0f, 0.0f, 0.0f, 1.0f };
            props.renderViewportRect             =
            {
                0.0f, static_cast<float>(m_pSetting->resolution.width),
                static_cast<float>(m_pSetting->resolution.height), 0.0f
            };
            props.viewportRects                  = { props.renderViewportRect };
            props.scissorRects                   = { props.renderViewportRect };

            // Initialize shaders
            Shader::BxShaderMeta mainSceneShaderMeta          = {};
            mainSceneShaderMeta.vertexShaderInfo.shaderFile   = "SimpleMesh.vert.spv";
            mainSceneShaderMeta.fragmentShaderInfo.shaderFile = "SimpleMesh.frag.spv";

            // Initialize render pass
            const UINT backbufferAttachmentNum = 1;
            const UINT backbufferNum           = static_cast<const UINT>(m_ppBackbufferTextures->size());

            std::vector<Render::VulkanRenderTargetCreateData>                         renderTargetsCreateData(backbufferAttachmentNum);
            std::vector<std::vector<Render::VulkanRenderTargetFramebufferCreateData>> renderTargetsFramebuffersCreateData(backbufferAttachmentNum);

            for (UINT i = 0; i < backbufferAttachmentNum; ++i)
            {
                Render::VulkanRenderTargetCreateData* pRenderPassCreateData = &(renderTargetsCreateData[i]);

                pRenderPassCreateData->renderSubPassIndex = 0;
                pRenderPassCreateData->bindingPoint       = 0;
                pRenderPassCreateData->isStore            = TRUE;
                pRenderPassCreateData->layout             = BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_PRESENT;
                pRenderPassCreateData->useStencil         = FALSE;
                pRenderPassCreateData->isStoreStencil     = FALSE;

                std::vector<Render::VulkanRenderTargetFramebufferCreateData>*
                    pRenderTargetsFramebufferCreateData = &(renderTargetsFramebuffersCreateData[i]);

                pRenderTargetsFramebufferCreateData->resize(backbufferNum);
                for (UINT j = 0; j < backbufferNum; ++j)
                {
                    pRenderTargetsFramebufferCreateData->at(j).framebufferIndex = j;
                    pRenderTargetsFramebufferCreateData->at(j).pTexture         = m_ppBackbufferTextures->at(j);
                }

                pRenderPassCreateData->pRenderTargetFramebufferCreateData = pRenderTargetsFramebufferCreateData;
            }

            Render::VulkanRenderResources renderSources = {};
            renderSources.vertexBufferTexChannelNum     = 1;
            renderSources.vertexDescriptionBindingPoint = 0;
            renderSources.pVertexInputResourceList      = &m_mainSceneVertexInputResourceList;

            Render::VulkanRenderPassCreateData mainSceneRenderPassCreateData = {};
            mainSceneRenderPassCreateData.pProps                             = &props;
            mainSceneRenderPassCreateData.pShaderMeta                        = &mainSceneShaderMeta;
            mainSceneRenderPassCreateData.pResource                          = &renderSources;
            mainSceneRenderPassCreateData.renderFramebufferNum               = backbufferNum;
            mainSceneRenderPassCreateData.renderSubPassNum                   = 1;
            mainSceneRenderPassCreateData.pRenderTargetCreateDataList        = &renderTargetsCreateData;

            m_mainSceneRenderPass.create(mainSceneRenderPassCreateData);

            return status;
        }
    
        void VulkanForwardRender::update(
            const float delta)
        {
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