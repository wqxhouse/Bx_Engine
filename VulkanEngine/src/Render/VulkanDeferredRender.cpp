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
#define GBUFFER_POS_BUFFER_INDEX       0
#define GBUFFER_NORMAL_BUFFER_INDEX    1
#define GBUFFER_TEXCOORD0_BUFFER_INDEX 2
#define GBUFFER_DEPTH_BUFFER_INDEX     3
#define GBUFFER_NUM                    (GBUFFER_DEPTH_BUFFER_INDEX + 1)

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
            std::vector<VulkanRenderTargetCreateDescriptor> deferredRenderRTDescList =
            {
                // isStore | renderSubPassIndex | bindingPoint | layout | useStencil isStoreStencil;
                { TRUE, 1, 0, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_PRESENT, FALSE, FALSE }, // Backbuffer
                { TRUE, 0, 0, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_COLOR,   FALSE, FALSE }, // Position
                { TRUE, 0, 1, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_COLOR,   FALSE, FALSE }, // Normal
                { TRUE, 0, 2, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_COLOR,   FALSE, FALSE }  // TexCoord
            };

            // Create textures
            createGBufferTextures();

            assert((IsDepthTestEnabled() == TRUE)  ||
                   ((IsDepthTestEnabled()   == FALSE) &&
                    (IsStencilTestEnabled() == FALSE)));

            // Create depth buffer
            if (IsDepthTestEnabled() == TRUE)
            {
                renderProperties.enableDepth     = TRUE;
                renderProperties.depthClearValue = { 1.0f, 0.0f };

                genBackbufferDepthBuffer();

                deferredRenderRTDescList.push_back({ TRUE, 0, 3, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_DEPTH_STENCIL, FALSE, FALSE });
            }

            if (IsStencilTestEnabled() == TRUE)
            {
                renderProperties.enableStencil     = TRUE;
                renderProperties.stencilClearValue = { 1.0f, 0.0f };
            }

            // Render Targets
            std::vector<std::vector<VulkanRenderTargetFramebufferCreateData>>
                deferredRenderRTFramebuffersCreateData(deferredRenderRTDescList.size());

            std::vector<VulkanRenderTargetCreateData> deferredRenderRTList =
                genRenderTargetsCreateData(deferredRenderRTDescList, &deferredRenderRTFramebuffersCreateData);

            // Initialize uniform buffers for render pass
            m_transUniformbuffer.resize(m_pScene->GetSceneModelNum());

            std::vector<VulkanRenderSubpassCreateData> deferredRenderSubpassCreateDataList(2);

            Shader::BxShaderMeta                    gBufferShaderMeta                 = {};
            VulkanRenderResources                   gBufferResources                  = {};
            VulkanGraphicsPipelineProperties        gBufferGraphicsPipelineProperties = {};
            VulkanSubpassGraphicsPipelineCreateData gBufferGraphicsPipelineCreateData = {};

            Shader::BxShaderMeta                      shadingPassShader                     = {};
            VulkanRenderResources                     shadingPassResources                  = {};
            VulkanGraphicsPipelineProperties          shadingPassGraphicsPipelineProps      = {};
            VulkanSubpassGraphicsPipelineCreateData   shadingPassGraphicsPipelineCreateData = {};

            /// GBuffer subpass
            std::vector<VulkanUniformBufferResource> transUniformBufferResource = createTransUniformBufferResource();

            std::vector<VulkanRenderTargetCreateData*> gBufferRTCreateDataRefList =
            {
                &(deferredRenderRTList[1]),
                &(deferredRenderRTList[2]),
                &(deferredRenderRTList[3]),
                &(deferredRenderRTList[4])
            };

            deferredRenderSubpassCreateDataList[0] = genGBufferSubpassCreateData(renderProperties,
                                                                                 &gBufferRTCreateDataRefList,
                                                                                 &gBufferShaderMeta,
                                                                                 &transUniformBufferResource,
                                                                                 &gBufferResources,
                                                                                 &gBufferGraphicsPipelineProperties,
                                                                                 &gBufferGraphicsPipelineCreateData);

            // Shading pass
            std::vector<VulkanRenderTargetCreateData*> shadingPassRTCreateDataRefList =
            {
                &(deferredRenderRTList[0]),
                &(deferredRenderRTList[4])
            };

            deferredRenderSubpassCreateDataList[1] = genShadingPassCreateData(renderProperties,
                                                                              &shadingPassRTCreateDataRefList,
                                                                              &shadingPassShader,
                                                                              &shadingPassResources,
                                                                              &shadingPassGraphicsPipelineProps,
                                                                              &shadingPassGraphicsPipelineCreateData);

            // Create dependencies
            std::vector<VkSubpassDependency> deferredRenderDependencies(1);
            deferredRenderDependencies[0].srcSubpass    = 0;
            deferredRenderDependencies[0].dstSubpass    = 1;
            deferredRenderDependencies[0].srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            deferredRenderDependencies[0].dstStageMask  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            deferredRenderDependencies[0].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            deferredRenderDependencies[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            VulkanRenderpassCreateData renderPassCreateData  = {};
            renderPassCreateData.pRenderProperties           = &renderProperties;
            renderPassCreateData.pSubpassCreateDataList      = &deferredRenderSubpassCreateDataList;
            renderPassCreateData.framebufferNum              = backbufferNum;
            renderPassCreateData.pRenderTargetCreateDataList = &deferredRenderRTList;
            renderPassCreateData.pSubpassDependencyList      = &deferredRenderDependencies;

            status = m_mainSceneRenderPass.create(renderPassCreateData);

            assert(status == BX_SUCCESS);

            return status;
        }

        BOOL VulkanDeferredRender::update(
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

            status = m_mainSceneRenderPass.update(deltaTime, { m_descriptorUpdateDataList, std::vector<VulkanDescriptorUpdateData>() });

            assert(status == BX_SUCCESS);

            for (VulkanRenderPass postRenderPass : m_postDrawPassList)
            {
                // status = postRenderPass.update(deltaTime);

                assert(status == BX_SUCCESS);
            }

            return status;
        }

        void VulkanDeferredRender::draw()
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

        void VulkanDeferredRender::createGBufferTextures()
        {
            Texture::VulkanTexture2D* pPosTexture = m_pTextureMgr->createTexture2DRenderTarget(m_pSetting->resolution.width,
                                                                                               m_pSetting->resolution.height,
                                                                                               m_pSetting->m_graphicsSetting.antialasing,
                                                                                               TextureFormat::BX_FORMAT_BGRA8);

            Texture::VulkanTexture2D* pNormalTexture = m_pTextureMgr->createTexture2DRenderTarget(m_pSetting->resolution.width,
                                                                                                  m_pSetting->resolution.height,
                                                                                                  m_pSetting->m_graphicsSetting.antialasing,
                                                                                                  TextureFormat::BX_FORMAT_BGRA8);

            Texture::VulkanTexture2D* pTexCoord0Texture = m_pTextureMgr->createTexture2DRenderTarget(m_pSetting->resolution.width,
                                                                                                    m_pSetting->resolution.height,
                                                                                                    m_pSetting->m_graphicsSetting.antialasing,
                                                                                                    TextureFormat::BX_FORMAT_BGRA8);

            size_t backbufferNum = m_backBufferRTsCreateDataList.size();
            for (size_t i = 0; i < backbufferNum; ++i)
            {
                m_backBufferRTsCreateDataList[i].push_back({ GBUFFER_POS_BUFFER_INDEX, pPosTexture });
                m_backBufferRTsCreateDataList[i].push_back({ GBUFFER_NORMAL_BUFFER_INDEX, pNormalTexture });
                m_backBufferRTsCreateDataList[i].push_back({ GBUFFER_TEXCOORD0_BUFFER_INDEX, pTexCoord0Texture });

            }
        }

        std::vector<VulkanRenderTargetCreateData> VulkanDeferredRender::genRenderTargetsCreateData(
            IN  const std::vector<VulkanRenderTargetCreateDescriptor>&             RTDescList,
            OUT std::vector<std::vector<VulkanRenderTargetFramebufferCreateData>>* pRTFrameBuffersCreateDataList)
        {
            const size_t RTSize = RTDescList.size();

            std::vector<VulkanRenderTargetCreateData> renderTargetCreateDataList(RTSize);

            const UINT backbufferNum = static_cast<const UINT>(m_backBufferRTsCreateDataList.size());

            for (size_t RTIndex = 0; RTIndex < RTSize; ++RTIndex)
            {
                const VulkanRenderTargetCreateDescriptor* pRTDesc = &(RTDescList[RTIndex]);

                VulkanRenderTargetCreateData* pRTCreateData = &(renderTargetCreateDataList[RTIndex]);

                pRTCreateData->renderSubPassIndex           = pRTDesc->renderSubPassIndex;
                pRTCreateData->bindingPoint                 = pRTDesc->bindingPoint;
                pRTCreateData->isStore                      = pRTDesc->isStore;
                pRTCreateData->layout                       = pRTDesc->layout;
                pRTCreateData->useStencil                   = pRTDesc->useStencil;
                pRTCreateData->isStoreStencil               = pRTDesc->isStoreStencil;

                std::vector<VulkanRenderTargetFramebufferCreateData>*
                    pRenderTargetsFramebufferCreateData = &(pRTFrameBuffersCreateDataList->at(RTIndex));

                pRenderTargetsFramebufferCreateData->resize(backbufferNum);
                for (UINT backbufferIndex = 0; backbufferIndex < backbufferNum; ++backbufferIndex)
                {
                    pRenderTargetsFramebufferCreateData->at(backbufferIndex) = m_backBufferRTsCreateDataList[backbufferIndex][RTIndex];
                }

                pRTCreateData->pRenderTargetFramebufferCreateData = pRenderTargetsFramebufferCreateData;
            }

            return renderTargetCreateDataList;
        }

        VulkanRenderSubpassCreateData VulkanDeferredRender::genGBufferSubpassCreateData(
            IN  const VulkanRenderProperties&                                      renderProps,
            IN  std::vector<VulkanRenderTargetCreateData*>*                        pRTCreateDataRefList,
            OUT Shader::BxShaderMeta*                                              pGBufferShaderMeta,
            OUT std::vector<VulkanUniformBufferResource>*                          pGBufferUniformBufferResourceList,
            OUT VulkanRenderResources*                                             pGBufferResources,
            OUT VulkanGraphicsPipelineProperties*                                  pGBufferGraphicsPipelineProperties,
            OUT VulkanSubpassGraphicsPipelineCreateData*                           pGBufferGraphicsPipelineCreateData)
        {
            VulkanRenderSubpassCreateData gBufferPassCreateData = {};

            //  GBuffer shaders
            pGBufferShaderMeta->vertexShaderInfo   = { "GBuffer.vert.spv", "main" };
            pGBufferShaderMeta->fragmentShaderInfo = { "GBuffer.frag.spv", "main" };

            pGBufferResources->vertexBufferTexChannelNum     = 1;
            pGBufferResources->vertexDescriptionBindingPoint = 0;
            pGBufferResources->pVertexInputResourceList      = &m_mainSceneVertexInputResourceList;

            // Initialize uniform buffers for render pass
            pGBufferResources->pUniformBufferResourceList = pGBufferUniformBufferResourceList;

            // For GBuffer pass, no need to use textures
            pGBufferResources->pTextureResouceList = NULL;

            /// Create gbuffer graphics pipeline
            pGBufferGraphicsPipelineProperties->cullMode      = CULLMODE_BACK;
            pGBufferGraphicsPipelineProperties->polyMode      = POLYMODE_FILL;
            pGBufferGraphicsPipelineProperties->viewportRects = { renderProps.renderViewportRect };
            pGBufferGraphicsPipelineProperties->scissorRects  = { renderProps.renderViewportRect };

            pGBufferGraphicsPipelineCreateData->subpassIndex    = 0;
            pGBufferGraphicsPipelineCreateData->pProps          = pGBufferGraphicsPipelineProperties;
            pGBufferGraphicsPipelineCreateData->pShaderMeta     = pGBufferShaderMeta;
            pGBufferGraphicsPipelineCreateData->pResource       = pGBufferResources;
            pGBufferGraphicsPipelineCreateData->isScenePipeline = TRUE;

            // Generate subpass graphics pipeline create data
            gBufferPassCreateData.pSubpassGraphicsPipelineCreateData    = pGBufferGraphicsPipelineCreateData;
            gBufferPassCreateData.pSubpassRenderTargetCreateDataRefList = pRTCreateDataRefList;

            return gBufferPassCreateData;
        }

        VulkanRenderSubpassCreateData VulkanDeferredRender::genShadingPassCreateData(
            IN const VulkanRenderProperties&                                       renderProps,
            IN  std::vector<VulkanRenderTargetCreateData*>*                        pRTCreateDataRefList,
            OUT Shader::BxShaderMeta*                                              pShadingPassShader,
            OUT VulkanRenderResources*                                             pShadingPassResources,
            OUT VulkanGraphicsPipelineProperties*                                  pShadingPassGraphicsPipelineProps,
            OUT VulkanSubpassGraphicsPipelineCreateData*                           pShadingPassGraphicsPipelineCreateData)
        {
            VulkanRenderSubpassCreateData shadingPassCreateData = {};

            const UINT backbufferNum = static_cast<const UINT>(m_backBufferRTsCreateDataList.size());

            // Backbuffer shader
            pShadingPassShader->vertexShaderInfo   = { "MainSceneDeferred.vert.spv", "main" };
            pShadingPassShader->fragmentShaderInfo = { "MainSceneDeferred.frag.spv", "main" };

            /// Create shading pass resources
            Buffer::VulkanVertexBuffer* pShadingPassQuadVertexBuffer =
                new Buffer::VulkanVertexBuffer(m_pDevice, m_pCmdBufferMgr, 4, 2 * sizeof(float), m_shadingPassQuad);
            pShadingPassQuadVertexBuffer->createVertexBuffer(*m_pHwDevice, TRUE);

            Buffer::VulkanIndexBuffer* pShadingPassQuadIndexBuffer =
                new Buffer::VulkanIndexBuffer(m_pDevice, m_pCmdBufferMgr, &m_shadingPassQuadIndices);
            pShadingPassQuadIndexBuffer->createIndexBuffer(*m_pHwDevice, TRUE);

            m_shadingPassVertexInput.resize(1);
            m_shadingPassVertexInput[0].pVertexBuffer = std::unique_ptr<Buffer::VulkanVertexBuffer>(pShadingPassQuadVertexBuffer);
            m_shadingPassVertexInput[0].pIndexBuffer  = std::unique_ptr<Buffer::VulkanIndexBuffer>(pShadingPassQuadIndexBuffer);

            pShadingPassResources->vertexBufferTexChannelNum     = 1;
            pShadingPassResources->vertexDescriptionBindingPoint = 0;
            pShadingPassResources->pVertexInputResourceList      = &m_shadingPassVertexInput;
            pShadingPassResources->pUniformBufferResourceList    = NULL;

            // Initialize textures for shading pass
            std::vector<VulkanTextureResource> sceneTextureResourceList = createSceneTextures();
            pShadingPassResources->pTextureResouceList                  = &sceneTextureResourceList;

            /// Create shading pass graphics pipeline
            pShadingPassGraphicsPipelineProps->cullMode      = CULLMODE_BACK;
            pShadingPassGraphicsPipelineProps->polyMode      = POLYMODE_FILL;
            pShadingPassGraphicsPipelineProps->viewportRects = { renderProps.renderViewportRect };
            pShadingPassGraphicsPipelineProps->scissorRects  = { renderProps.renderViewportRect };

            pShadingPassGraphicsPipelineCreateData->subpassIndex    = 1;
            pShadingPassGraphicsPipelineCreateData->pProps          = pShadingPassGraphicsPipelineProps;
            pShadingPassGraphicsPipelineCreateData->pShaderMeta     = pShadingPassShader;
            pShadingPassGraphicsPipelineCreateData->pResource       = pShadingPassResources;
            pShadingPassGraphicsPipelineCreateData->isScenePipeline = FALSE;

            /// Generate shading pass create data
            shadingPassCreateData.pSubpassGraphicsPipelineCreateData    = pShadingPassGraphicsPipelineCreateData;
            shadingPassCreateData.pSubpassRenderTargetCreateDataRefList = pRTCreateDataRefList;

            return shadingPassCreateData;
        }
    }
}