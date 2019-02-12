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
            std::vector<VulkanRenderTargetCreateDescriptor> deferredRenderRTs =
            {
                // isStore | renderSubPassIndex | bindingPoint | layout | useStencil isStoreStencil;
                { TRUE, 0, 0, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_PRESENT, FALSE, FALSE }, // Backbuffer
                { TRUE, 0, 0, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_COLOR,   FALSE, FALSE }, // Position
                { TRUE, 0, 0, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_COLOR,   FALSE, FALSE }, // Normal
                { TRUE, 0, 0, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_COLOR,   FALSE, FALSE }  // TexCoord
            };

            assert((IsDepthTestEnabled() == TRUE)  ||
                   ((IsDepthTestEnabled()   == FALSE) &&
                    (IsStencilTestEnabled() == FALSE)));

            // Create depth buffer
            if (IsDepthTestEnabled() == TRUE)
            {
                renderProperties.enableDepth     = TRUE;
                renderProperties.depthClearValue = { 1.0f, 0.0f };

                size_t backBufferNum = m_backBufferRTsCreateDataList.size();

                for (size_t i = 0; i < backBufferNum; ++i)
                {
                    Texture::VulkanTexture2D* pBackbufferColorTexture =
                        static_cast<Texture::VulkanTexture2D*>(m_backBufferRTsCreateDataList[0][0].pTexture);

                    TextureFormat depthBufferFormat =
                        ((IsStencilTestEnabled() == FALSE) ? BX_FORMAT_DEPTH32 : BX_FORMAT_DEPTH24_STENCIL);

                    Texture::VulkanTexture2D * backbufferDepthTexture =
                        m_pTextureMgr->createTexture2DRenderTarget(
                            pBackbufferColorTexture->GetTextureWidth(),
                            pBackbufferColorTexture->GetTextureHeight(),
                            static_cast<UINT>(m_pSetting->m_graphicsSetting.antialasing),
                            depthBufferFormat);

                    m_backBufferRTsCreateDataList[i].push_back({ static_cast<UINT>(i), backbufferDepthTexture });
                }

                deferredRenderRTs.push_back({ TRUE, 0, 1, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_DEPTH_STENCIL, FALSE, FALSE });
            }

            if (IsStencilTestEnabled() == TRUE)
            {
                renderProperties.enableStencil = TRUE;
                renderProperties.stencilClearValue = { 1.0f, 0.0f };
            }

            /// GBuffer subpass
            {
                //  GBuffer shaders
                Shader::BxShaderMeta gBufferShaderMeta = {};
                gBufferShaderMeta.vertexShaderInfo     = { "GBuffer.vert.spv", "main" };
                gBufferShaderMeta.fragmentShaderInfo   = { "GBuffer.frag.spv", "main" };
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