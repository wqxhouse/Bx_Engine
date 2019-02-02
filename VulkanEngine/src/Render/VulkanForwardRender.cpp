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
            props.cullMode               = CULLMODE_BACK;
            props.enableBlending         = TRUE;
            props.polyMode               = PolyMode::POLYMODE_FILL;
            props.sceneClearValue        = { 0.0f, 0.0f, 0.0f, 1.0f };
            props.renderViewportRect     =
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

            /// Initialize render pass

            // Create depth buffer
            assert((IsDepthTestEnabled() == TRUE)  ||
                   ((IsDepthTestEnabled()   == FALSE) &&
                    (IsStencilTestEnabled() == FALSE)));

            /*if (IsDepthTestEnabled() == TRUE)
            {
                Texture::VulkanTexture2D* pBackbufferColorTexture = m_ppBackbufferTextures->at(0);

                TextureFormat depthBufferFormat =
                    ((IsStencilTestEnabled() == FALSE) ? BX_FORMAT_DEPTH32 : BX_FORMAT_DEPTH24_STENCIL);

                Texture::VulkanTexture2D* backbufferDepthTexture =
                    m_pTextureMgr->createTexture2DRenderTarget(
                        pBackbufferColorTexture->GetTextureWidth(),
                        pBackbufferColorTexture->GetTextureHeight(),
                        static_cast<UINT>(m_pSetting->m_graphicsSetting.antialasing),
                        depthBufferFormat);

                m_ppBackbufferTextures->push_back(backbufferDepthTexture);
            }*/

            // Generate render pass create data
            const UINT backbufferAttachmentNum = 1;
            const UINT backbufferNum           = static_cast<const UINT>(m_backBufferRTsCreateDataList.size());

            std::vector<VulkanRenderTargetCreateDescriptor>                   renderTargetDescriptors(backbufferAttachmentNum);

            //renderTargetDescriptors

            std::vector<VulkanRenderTargetCreateData>                         renderTargetsCreateData(backbufferAttachmentNum);
            std::vector<std::vector<VulkanRenderTargetFramebufferCreateData>> renderTargetsFramebuffersCreateData(backbufferAttachmentNum);

            for (UINT i = 0; i < backbufferAttachmentNum; ++i)
            {
                VulkanRenderTargetCreateData* pRenderTargetCreateData = &(renderTargetsCreateData[i]);

                pRenderTargetCreateData->renderSubPassIndex = 0;
                pRenderTargetCreateData->bindingPoint       = 0;
                pRenderTargetCreateData->isStore            = TRUE;
                pRenderTargetCreateData->layout             = BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_PRESENT;
                pRenderTargetCreateData->useStencil         = FALSE;
                pRenderTargetCreateData->isStoreStencil     = FALSE;

                std::vector<VulkanRenderTargetFramebufferCreateData>*
                    pRenderTargetsFramebufferCreateData = &(renderTargetsFramebuffersCreateData[i]);

                pRenderTargetsFramebufferCreateData->resize(backbufferNum);
                for (UINT j = 0; j < backbufferNum; ++j)
                {
                    pRenderTargetsFramebufferCreateData->at(j) = m_backBufferRTsCreateDataList[i][j];
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
            const VkPhysicalDeviceProperties hwProps = Utility::VulkanUtility::GetHwProperties(*m_pHwDevice);

            m_forwardRenderMainSceneUbo.resize(m_pScene->GetSceneModelNum());

            Buffer::VulkanUniformBufferDynamic* pMainSceneUniformbuffer =
                new Buffer::VulkanUniformBufferDynamic(m_pDevice,
                                                       hwProps.limits.minUniformBufferOffsetAlignment);

            pMainSceneUniformbuffer->createUniformBuffer(*m_pHwDevice,
                                                         (UINT)m_forwardRenderMainSceneUbo.size(),
                                                         sizeof(m_forwardRenderMainSceneUbo[0]),
                                                         m_forwardRenderMainSceneUbo.data());

            m_pDescriptorBufferList.push_back(
                std::unique_ptr<Buffer::VulkanDescriptorBuffer>(pMainSceneUniformbuffer));

            std::vector<VulkanUniformBufferResource> mainSceneUniformbufferResourceList(1);
            mainSceneUniformbufferResourceList[0].shaderType       = BX_VERTEX_SHADER;
            mainSceneUniformbufferResourceList[0].bindingPoint     = 0;
            mainSceneUniformbufferResourceList[0].uniformbufferNum = 1;
            mainSceneUniformbufferResourceList[0].pUniformBuffer   =
                static_cast<Buffer::VulkanUniformBufferDynamic*>(m_pDescriptorBufferList[0].get());

            renderSources.pUniformBufferResourceList = &mainSceneUniformbufferResourceList;

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

            std::vector<VulkanTextureResource> mainSceneTextureResourceList(1);
            mainSceneTextureResourceList[0].shaderType       = BX_FRAGMENT_SHADER;
            mainSceneTextureResourceList[0].bindingPoint     = 1;
            mainSceneTextureResourceList[0].textureNum       = 1;
            mainSceneTextureResourceList[0].pTexture         = pTexture;

            renderSources.pTextureResouceList = &mainSceneTextureResourceList;

            // Create render pass create data
            VulkanRenderPassCreateData mainSceneRenderPassCreateData = {};
            mainSceneRenderPassCreateData.pProps                             = &props;
            mainSceneRenderPassCreateData.pShaderMeta                        = &mainSceneShaderMeta;
            mainSceneRenderPassCreateData.pResource                          = &renderSources;
            mainSceneRenderPassCreateData.renderFramebufferNum               = backbufferNum;
            mainSceneRenderPassCreateData.renderSubPassNum                   = 1;
            mainSceneRenderPassCreateData.pRenderTargetCreateDataList        = &renderTargetsCreateData;

            m_mainSceneRenderPass.create(mainSceneRenderPassCreateData);

            return status;
        }

        BOOL VulkanForwardRender::update(
            const float deltaTime)
        {
            BOOL status = BX_SUCCESS;

            for (VulkanRenderPass preRenderPass : m_preDrawPassList)
            {
                status = preRenderPass.update(deltaTime);

                assert(status == BX_SUCCESS);
            }

            status = m_mainSceneRenderPass.update(deltaTime);

            assert(status == BX_SUCCESS);

            for (VulkanRenderPass postRenderPass : m_postDrawPassList)
            {
                status = postRenderPass.update(deltaTime);

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