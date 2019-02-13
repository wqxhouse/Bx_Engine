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

            /// GBuffer subpass
            {
                //  GBuffer shaders
                Shader::BxShaderMeta gBufferShaderMeta = {};
                gBufferShaderMeta.vertexShaderInfo     = { "GBuffer.vert.spv", "main" };
                gBufferShaderMeta.fragmentShaderInfo   = { "GBuffer.frag.spv", "main" };

                // GBuffer attachments
                size_t gbufferRTNum = deferredRenderRTList.size() - 1;

                std::vector<VulkanRenderTargetCreateData> gBufferRTCreateDataList(gbufferRTNum);

                for (UINT i = 0; i < gbufferRTNum; ++i)
                {
                    VulkanRenderTargetCreateData* pGBufferRTCreateData         = &gBufferRTCreateDataList[i];
                    const VulkanRenderTargetCreateDescriptor& deferredRenderRT = deferredRenderRTList[i + 1];

                    pGBufferRTCreateData->renderSubPassIndex = deferredRenderRT.renderSubPassIndex;
                    pGBufferRTCreateData->bindingPoint       = deferredRenderRT.bindingPoint;
                    pGBufferRTCreateData->isStore            = deferredRenderRT.isStore;
                    pGBufferRTCreateData->layout             = deferredRenderRT.layout;
                    pGBufferRTCreateData->useStencil         = deferredRenderRT.useStencil;
                    pGBufferRTCreateData->isStoreStencil     = deferredRenderRT.isStoreStencil;
                }

                Buffer::VulkanUniformBufferDynamic* pGBufferUniformbuffer =
                    new Buffer::VulkanUniformBufferDynamic(m_pDevice,
                                                           hwProps.limits.minUniformBufferOffsetAlignment);

                pGBufferUniformbuffer->createUniformBuffer(*m_pHwDevice,
                                                           (UINT)m_transUniformbuffer.size(),
                                                           sizeof(m_transUniformbuffer[0]),
                                                           m_transUniformbuffer.data());

                VulkanRenderResources renderSources         = {};
                renderSources.vertexBufferTexChannelNum     = 1;
                renderSources.vertexDescriptionBindingPoint = 0;
                renderSources.pVertexInputResourceList      = &m_mainSceneVertexInputResourceList;
            }

            // Main Render subpass
            {

            }

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
    }
}