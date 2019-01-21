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
            const Setting*            pSetting,
            const VkDevice*           pDevice,
            Mgr::CmdBufferMgr*        pCmdBufferMgr,
            Mgr::DescriptorMgr*       pDescritorMgr,
            const Scene::RenderScene* pScene)
            : VulkanRenderBase(pSetting, pDevice, pCmdBufferMgr, pDescritorMgr, pScene)
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
            props.cullMode                       = CULLMODE_BACK;
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
            mainSceneShaderMeta.vertexShaderInfo.shaderFile   = "SimpleMesh.vert";
            mainSceneShaderMeta.fragmentShaderInfo.shaderFile = "SimpleMesh.frag";

            // Initialize render pass
            UINT backbufferNum = static_cast<UINT>(m_ppBackbufferTextures->size());

            std::vector<Render::VulkanRenderTargetsCreateData> renderTargetsCreateData(backbufferNum);

            for (UINT i = 0; i < backbufferNum; ++i)
            {
                Render::VulkanRenderTargetsCreateData* pRenderPassCreateData = &(renderTargetsCreateData[i]);

                pRenderPassCreateData->framebufferIndex   = i;
                pRenderPassCreateData->renderSubPassIndex = 0;
                pRenderPassCreateData->bindingPoint       = 0;
                pRenderPassCreateData->isStore            = TRUE;
                pRenderPassCreateData->pTexture           = m_ppBackbufferTextures->at(i);
                pRenderPassCreateData->layout             = BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_COLOR;
                pRenderPassCreateData->useStencil         = FALSE;
                pRenderPassCreateData->isStoreStencil     = FALSE;
            }

            Render::VulkanRenderResources renderSources = {};
            renderSources.vertexBufferTexChannelNum     = 1;
            renderSources.vertexDescriptionBindingPoint = 0;
            renderSources.pVertexInputResourceList      = &m_mainSceneVertexInputResourceList;

            Render::VulkanRenderPassCreateData mainSceneRenderPassCreateData = {};
            mainSceneRenderPassCreateData.pProps                      = &props;
            mainSceneRenderPassCreateData.pShaderMeta                 = &mainSceneShaderMeta;
            mainSceneRenderPassCreateData.pResource                   = &renderSources;
            mainSceneRenderPassCreateData.renderFramebufferNum        = backbufferNum;
            mainSceneRenderPassCreateData.renderSubPassNum            = 1;
            mainSceneRenderPassCreateData.pRenderTargetCreateDataList = &renderTargetsCreateData;

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