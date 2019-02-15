//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "VulkanRender.h"

/*
    GBuffer Structure
    0: Position Buffer
    1: Normal   Buffer
    2: TexCoord Buffer
    3: Depth    Buffer
*/

namespace VulkanEngine
{
    namespace Render
    {
        VulkanDeferredRender::VulkanDeferredRender(
            const Setting* const                                pSetting,
            const VkPhysicalDevice* const                       pHwDevice,
            const VkDevice* const                               pDevice,
            Mgr::CmdBufferMgr* const                            pCmdBufferMgr,
            Mgr::DescriptorMgr* const                           pDescritorMgr,
            Mgr::TextureMgr* const                              pTextureMgr,
            const Scene::RenderScene*                           pScene,
            const std::vector<Texture::VulkanTexture2D*>* const ppBackbufferTextures)
            : VulkanRenderBase(pSetting, pHwDevice, pDevice, pCmdBufferMgr, pDescritorMgr, pTextureMgr,
                               pScene, ppBackbufferTextures)
        {

        }

        VulkanDeferredRender::~VulkanDeferredRender()
        {
        }

        BOOL VulkanDeferredRender::initialize()
        {
            BOOL status = BX_SUCCESS;

            const UINT backbufferNum =
                static_cast<const UINT>(m_backBufferRTsCreateDataList.size());

            const VkPhysicalDeviceProperties hwProps =
                Utility::VulkanUtility::GetHwProperties(*m_pHwDevice);

            VulkanRenderProperties renderProperties = {};
            renderProperties.enableBlending         = TRUE;
            renderProperties.sceneClearValue        =
            {
                { 0.0f, 0.0f, 0.0f, 1.0f },
                { 0.0f, 0.0f, 0.0f, 1.0f },
                { 0.0f, 0.0f, 0.0f, 1.0f },
                { 0.0f, 0.0f, 0.0f, 1.0f }
            };
            renderProperties.renderViewportRect     =
            {
                0.0f, static_cast<float>(m_pSetting->resolution.width),
                static_cast<float>(m_pSetting->resolution.height), 0.0f
            };
            renderProperties.enableColor            = TRUE;

            // Render Targets Descriptors
            std::vector<VulkanRenderTargetCreateDescriptor> deferredRenderRTList =
            {
                // isStore | renderSubPassIndex | bindingPoint | layout | useStencil isStoreStencil;
                { TRUE, 0, 0, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_PRESENT, FALSE, FALSE }, // Backbuffer
                { TRUE, 0, 0, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_COLOR,   FALSE, FALSE }, // Position
                { TRUE, 0, 1, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_COLOR,   FALSE, FALSE }, // Normal
                { TRUE, 0, 2, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_COLOR,   FALSE, FALSE }  // TexCoord
            };

            assert((IsDepthTestEnabled() == TRUE)  ||
                   ((IsDepthTestEnabled()   == FALSE) &&
                    (IsStencilTestEnabled() == FALSE)));

            // Create depth buffer
            if (IsDepthTestEnabled() == TRUE)
            {
                renderProperties.enableDepth     = TRUE;
                renderProperties.depthClearValue = { 1.0f, 0.0f };

                genBackbufferDepthBuffer();

                deferredRenderRTList.push_back({ TRUE, 0, 3, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_DEPTH_STENCIL, FALSE, FALSE });
            }

            if (IsStencilTestEnabled() == TRUE)
            {
                renderProperties.enableStencil     = TRUE;
                renderProperties.stencilClearValue = { 1.0f, 0.0f };
            }

            // Initialize uniform buffers for render pass
            m_transUniformbuffer.resize(m_pScene->GetSceneModelNum());

            std::vector<VulkanRenderSubpassCreateData> deferredRenderSubpassCreateDataList(2);
            /// GBuffer subpass
            {
                deferredRenderSubpassCreateDataList.push_back(
                    genGBufferSubpassCreateData(renderProperties, deferredRenderRTList, 1, 4));
            }

            // Main Render subpass
            {

            }

            VulkanRenderpassCreateData renderPassCreateData = {};
            renderPassCreateData.pRenderProperties          = &renderProperties;
            renderPassCreateData.pSubpassCreateDataList     = &deferredRenderSubpassCreateDataList;
            renderPassCreateData.framebufferNum             = backbufferNum;

            status = m_mainSceneRenderPass.create(renderPassCreateData);

            assert(status == BX_SUCCESS);

            return status;
        }

        BOOL VulkanDeferredRender::update(
            const float delta)
        {
            return 0;
        }

        void VulkanDeferredRender::draw()
        {
        }

        VulkanRenderSubpassCreateData VulkanDeferredRender::genGBufferSubpassCreateData(
            const VulkanRenderProperties&                          renderProps,
            const std::vector<VulkanRenderTargetCreateDescriptor>& rtList,
            const UINT                                             startIndex,
            const UINT                                             gbufferRTNum)
        {
            VulkanRenderSubpassCreateData gBufferPassCreateData = {};

            //  GBuffer shaders
            Shader::BxShaderMeta gBufferShaderMeta = {};
            gBufferShaderMeta.vertexShaderInfo     = { "GBuffer.vert.spv", "main" };
            gBufferShaderMeta.fragmentShaderInfo   = { "GBuffer.frag.spv", "main" };

            // GBuffer attachments
            std::vector<VulkanRenderTargetCreateData> gBufferRTCreateDataList(gbufferRTNum);

            for (UINT gBufferIndex = 0, rtIndex = startIndex; gBufferIndex < gbufferRTNum; ++gBufferIndex, ++rtIndex)
            {
                VulkanRenderTargetCreateData* pGBufferRTCreateData         = &gBufferRTCreateDataList[gBufferIndex];
                const VulkanRenderTargetCreateDescriptor& deferredRenderRT = rtList[rtIndex];

                pGBufferRTCreateData->renderSubPassIndex = deferredRenderRT.renderSubPassIndex;
                pGBufferRTCreateData->bindingPoint       = deferredRenderRT.bindingPoint;
                pGBufferRTCreateData->isStore            = deferredRenderRT.isStore;
                pGBufferRTCreateData->layout             = deferredRenderRT.layout;
                pGBufferRTCreateData->useStencil         = deferredRenderRT.useStencil;
                pGBufferRTCreateData->isStoreStencil     = deferredRenderRT.isStoreStencil;
            }

            VulkanRenderResources renderSources         = {};
            renderSources.vertexBufferTexChannelNum     = 1;
            renderSources.vertexDescriptionBindingPoint = 0;
            renderSources.pVertexInputResourceList      = &m_mainSceneVertexInputResourceList;

            // Initialize uniform buffers for render pass
            std::vector<VulkanUniformBufferResource> gBufferUniformBufferResourceList = createTransUniformBufferResource();
            renderSources.pUniformBufferResourceList                                  = &gBufferUniformBufferResourceList;

            // Initialize textures for render pass
            std::vector<VulkanTextureResource> sceneTextureResourceList = createSceneTextures();
            renderSources.pTextureResouceList                           = &sceneTextureResourceList;

            VulkanGraphicsPipelineProperties gBufferGraphicsPipelineProperties = {};
            gBufferGraphicsPipelineProperties.cullMode                         = CULLMODE_BACK;
            gBufferGraphicsPipelineProperties.polyMode                         = PolyMode::POLYMODE_FILL;
            gBufferGraphicsPipelineProperties.viewportRects                    = { renderProps.renderViewportRect };
            gBufferGraphicsPipelineProperties.scissorRects                     = { renderProps.renderViewportRect };

            VulkanSubpassGraphicsPipelineCreateData gBufferGraphicsPipelineCreateData = {};
            gBufferGraphicsPipelineCreateData.subpassIndex                            = 0;
            gBufferGraphicsPipelineCreateData.pProps                                  = &gBufferGraphicsPipelineProperties;
            gBufferGraphicsPipelineCreateData.pShaderMeta                             = &gBufferShaderMeta;
            gBufferGraphicsPipelineCreateData.pResource                               = &renderSources;

            gBufferPassCreateData.pSubpassGraphicsPipelineCreateData = &gBufferGraphicsPipelineCreateData;
            gBufferPassCreateData.pRenderTargetCreateDataList        = &gBufferRTCreateDataList;

            return gBufferPassCreateData;
        }

        VulkanRenderSubpassCreateData VulkanDeferredRender::genMainPassCreateData(
            const VulkanRenderProperties&             renderProps,
            const VulkanRenderTargetCreateDescriptor& backBufferRTDesc)
        {
            VulkanRenderSubpassCreateData mainPassCreateData = {};

            // Backbuffer shader
            Shader::BxShaderMeta mainPassShader = {};
            mainPassShader.vertexShaderInfo     = { "MainSceneDeferred.vert.spv", "main" };
            mainPassShader.fragmentShaderInfo   = { "MainSceneDeferred.frag.spv", "main" };

            // Backbuffer attachments
            VulkanRenderTargetCreateData backbufferRTCreateData = {};
            backbufferRTCreateData.renderSubPassIndex           = 1;
            backbufferRTCreateData.bindingPoint                 = 0;
            backbufferRTCreateData.isStore                      = backBufferRTDesc.isStore;
            backbufferRTCreateData.layout                       = backBufferRTDesc.layout;
            backbufferRTCreateData.useStencil                   = backBufferRTDesc.useStencil;
            backbufferRTCreateData.isStoreStencil               = backBufferRTDesc.isStoreStencil;


            NotImplemented();
            Buffer::VulkanVertexBuffer mainPassVertexBuffer(m_pDevice, m_pCmdBufferMgr, NULL);
            Buffer::VulkanIndexBuffer  mainPassIndexBuffer(m_pDevice, m_pCmdBufferMgr, NULL);

            std::vector<VulkanVertexInputResource> mainPassVertexInputResource(1);
            mainPassVertexInputResource[0].pVertexBuffer = std::unique_ptr<Buffer::VulkanVertexBuffer>(&mainPassVertexBuffer);
            mainPassVertexInputResource[0].pIndexBuffer  = std::unique_ptr<Buffer::VulkanIndexBuffer>(&mainPassIndexBuffer);

            return mainPassCreateData;
        }
    }
}