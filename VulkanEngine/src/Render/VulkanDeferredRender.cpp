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
    Attachment list
    0: Backbuffer
    1-3: GBuffer

    NO_MSAA
    4: DepthBuffer

    MSAA
    4: MSAA Backbuffer
    5: DepthBuffer

    The depth buffer is always at the last index for
    easier to be cleared.
*/

#define BACK_BUFFER_INDEX                 0
/*
    GBuffer Structure
    0: Position Buffer
    1: Normal   Buffer
    2: TexCoord Buffer
    3: Depth    Buffer
*/
#define GBUFFER_POS_BUFFER_INDEX          1
#define GBUFFER_NORMAL_BUFFER_INDEX       2
#define GBUFFER_ALBEDO_BUFFER_INDEX       3
#define GBUFFER_DEPTH_BUFFER_INDEX        4
#define GBUFFER_NUM                       (GBUFFER_DEPTH_BUFFER_INDEX - GBUFFER_POS_BUFFER_INDEX + 1)

// Set 0
#define TRANSFORM_MATRIX_UBO_INDEX        0
#define MATERIAL_UBO_INDEX                1
#define ALBEDO_TEXTURE_INDEX              2

// Set 1
#define GBUFFER_POS_TEX_INDEX             0
#define GBUFFER_NORMAL_TEX_INDEX          1
#define GBUFFER_ALBEDO_TEX_INDEX          2
#define LIGHT_UBO_INDEX                   3
#define CAM_UBO_INDEX                     4
#define VIEW_MATRIX_UBO_INDEX             5
#define MSAA_UBO_INDEX                    6

#define DESCRIPTOR_SET_NUM                2

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
            m_descriptorSetNum = DESCRIPTOR_SET_NUM;
            m_descriptorUpdateDataTable.resize(DESCRIPTOR_SET_NUM);
        }

        VulkanDeferredRender::~VulkanDeferredRender()
        {
        }

        BOOL VulkanDeferredRender::initialize()
        {
            BOOL status = BX_SUCCESS;

            // Initialize backbuffer render targets
            initializeBackbufferRTCreateData();

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
            const UINT sampleNum = m_pSetting->m_graphicsSetting.antialasing;

            std::vector<VulkanRenderTargetCreateDescriptor> deferredRenderRTDescList =
            {
                // isStore | renderSubPassIndex | bindingPoint | sampleNum | layout | useStencil isStoreStencil;
                { TRUE, 1, 0, 1,         BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_PRESENT, FALSE, FALSE }, // Backbuffer
                { TRUE, 0, 0, sampleNum, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_COLOR,   FALSE, FALSE }, // Position
                { TRUE, 0, 1, sampleNum, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_COLOR,   FALSE, FALSE }, // Normal
                { TRUE, 0, 2, sampleNum, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_COLOR,   FALSE, FALSE }  // TexCoord
            };

            // Create textures
            createGBufferTextures();

            assert((IsDepthTestEnabled()    == TRUE)  ||
                   ((IsDepthTestEnabled()   == FALSE) &&
                    (IsStencilTestEnabled() == FALSE)));

            // Create depth buffer
            if (IsDepthTestEnabled() == TRUE)
            {
                renderProperties.enableDepth     = TRUE;
                renderProperties.depthClearValue = { 1.0f, 0.0f };

                genBackbufferDepthBuffer();

                deferredRenderRTDescList.push_back(
                    { TRUE, 0, 3, sampleNum, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_DEPTH_STENCIL, FALSE, FALSE });
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

            // GBuffer pass create data
            Shader::BxShaderMeta                    gBufferShaderMeta                 = {};
            VulkanRenderResources                   gBufferResources                  = {};

            VkVertexInputBindingDescription         gBufferVertexInputBindingDescription = {};
            std::vector<VkVertexInputAttributeDescription> gBufferVertexAttributeDescription;

            VulkanGraphicsPipelineProperties        gBufferGraphicsPipelineProperties    = {};
            VulkanSubpassGraphicsPipelineCreateData gBufferGraphicsPipelineCreateData    = {};

            // Shading pass create data
            Shader::BxShaderMeta                      shadingPassShader                        = {};
            VulkanRenderResources                     shadingPassResources                     = {};

            VkVertexInputBindingDescription           shadingPassVertexInputBindingDescription = {};
            std::vector<VkVertexInputAttributeDescription> shadingPassVertexAttributeDescription;

            VulkanGraphicsPipelineProperties          shadingPassGraphicsPipelineProps         = {};
            VulkanSubpassGraphicsPipelineCreateData   shadingPassGraphicsPipelineCreateData    = {};

            /// GBuffer subpass
            std::vector<VulkanUniformBufferResource>   gBufferTransUniformBufferResource = createGbufferUniformBufferResource();

            std::vector<VulkanTextureResource>         gBufferTextureResourceList;

            std::vector<VulkanDescriptorResources>     gBufferDescriptorResourcesList(1);

            std::vector<VulkanRenderTargetCreateData*> gBufferRTCreateDataRefList =
            {
                &(deferredRenderRTList[GBUFFER_POS_BUFFER_INDEX]),
                &(deferredRenderRTList[GBUFFER_NORMAL_BUFFER_INDEX]),
                &(deferredRenderRTList[GBUFFER_ALBEDO_BUFFER_INDEX])
            };

            if (IsDepthTestEnabled() == TRUE)
            {
                gBufferRTCreateDataRefList.push_back(&(deferredRenderRTList[GBUFFER_DEPTH_BUFFER_INDEX]));
            }

            std::vector<VulkanGraphicsPipelineRenderTargetProperties> gBufferGraphicsPipelineRTProps(GBUFFER_NUM - 1);

            deferredRenderSubpassCreateDataList[0] = genGBufferSubpassCreateData(renderProperties,
                                                                                 &gBufferRTCreateDataRefList,
                                                                                 &gBufferShaderMeta,
                                                                                 &gBufferTransUniformBufferResource,
                                                                                 &gBufferTextureResourceList,
                                                                                 &gBufferDescriptorResourcesList,
                                                                                 &gBufferResources,
                                                                                 &gBufferGraphicsPipelineRTProps,
                                                                                 &gBufferVertexInputBindingDescription,
                                                                                 &gBufferVertexAttributeDescription,
                                                                                 &gBufferGraphicsPipelineProperties,
                                                                                 &gBufferGraphicsPipelineCreateData);

            // Shading pass
            std::vector<VulkanUniformBufferResource>   shadingPassUniformBufferResource = createShadingPassUniformBufferResource();

            std::vector<VulkanRenderTargetCreateData*> shadingPassRTCreateDataRefList = { &(deferredRenderRTList[BACK_BUFFER_INDEX]) };

            std::vector<VulkanTextureResource> shadingPassTextureResourceList;
            std::vector<VulkanTextureResource> shadingPassInputAttachmentResourceList;

            std::vector<VulkanDescriptorResources> shadingPassDescriptorResourcesList(1);

            std::vector<VulkanGraphicsPipelineRenderTargetProperties> shadingPassGraphicsPipelineRTProps(1);

            deferredRenderSubpassCreateDataList[1] = genShadingPassCreateData(renderProperties,
                                                                              &shadingPassRTCreateDataRefList,
                                                                              &shadingPassShader,
                                                                              &shadingPassUniformBufferResource,
                                                                              &shadingPassTextureResourceList,
                                                                              &shadingPassInputAttachmentResourceList,
                                                                              &shadingPassDescriptorResourcesList,
                                                                              &shadingPassResources,
                                                                              &shadingPassGraphicsPipelineRTProps,
                                                                              &shadingPassVertexInputBindingDescription,
                                                                              &shadingPassVertexAttributeDescription,
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

            status = m_mainSceneRenderPass.update(deltaTime, m_descriptorUpdateDataTable);

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

        void VulkanDeferredRender::initializeBackbufferRTCreateData()
        {
            UINT backBufferTextureSize = 4;

            const size_t backBufferNum  = m_ppBackbufferTextures->size();
            for (size_t backBufferIndex = 0; backBufferIndex < backBufferNum; ++backBufferIndex)
            {
                m_backBufferRTsCreateDataList[backBufferIndex].resize(backBufferTextureSize);

                Texture::VulkanTextureBase* pBackbufferTexture = m_ppBackbufferTextures->at(backBufferIndex);

                m_backBufferRTsCreateDataList[backBufferIndex][BACK_BUFFER_INDEX] =
                {
                    static_cast<UINT>(backBufferIndex),
                    { 1, pBackbufferTexture }
                };
            }
        }

        void VulkanDeferredRender::createGBufferTextures()
        {
            const UINT sampleNum = m_pSetting->m_graphicsSetting.antialasing;

            TextureUsage usage = BX_TEXTURE_USAGE_NONE;

            if (m_pSetting->m_graphicsSetting.IsEnableAntialasing() == FALSE)
            {
                usage = BX_TEXTURE_USAGE_VULKAN_INPUT_ATTACHMENT;
            }
            else
            {
                usage = BX_TEXTURE_USAGE_SAMPLED;
            }

            Texture::VulkanTexture2D* pPosTexture =
                m_pTextureMgr->createTexture2DRenderTarget(m_pSetting->resolution.width,
                                                           m_pSetting->resolution.height,
                                                           sampleNum,
                                                           BX_FORMAT_RGBA32_FLOAT,
                                                           usage,
                                                           FALSE);

            Texture::VulkanTexture2D* pNormalTexture =
                m_pTextureMgr->createTexture2DRenderTarget(m_pSetting->resolution.width,
                                                           m_pSetting->resolution.height,
                                                           sampleNum,
                                                           BX_FORMAT_RGBA32_FLOAT,
                                                           usage,
                                                           FALSE);

            Texture::VulkanTexture2D* pTexCoord0Texture =
                m_pTextureMgr->createTexture2DRenderTarget(m_pSetting->resolution.width,
                                                           m_pSetting->resolution.height,
                                                           sampleNum,
                                                           BX_FORMAT_RGBA32_FLOAT,
                                                           usage,
                                                           FALSE);

            UINT backbufferNum = static_cast<UINT>(m_backBufferRTsCreateDataList.size());
            for (UINT backbufferIndex = 0; backbufferIndex < backbufferNum; ++backbufferIndex)
            {
                m_backBufferRTsCreateDataList[backbufferIndex][GBUFFER_POS_BUFFER_INDEX] =
                {
                    backbufferIndex,
                    { sampleNum, pPosTexture }
                };

                m_backBufferRTsCreateDataList[backbufferIndex][GBUFFER_NORMAL_BUFFER_INDEX] =
                {
                    backbufferIndex,
                    { sampleNum, pNormalTexture }
                };

                m_backBufferRTsCreateDataList[backbufferIndex][GBUFFER_ALBEDO_BUFFER_INDEX] =
                {
                    backbufferIndex,
                    { sampleNum, pTexCoord0Texture }
                };
            }
        }

        VulkanUniformBufferResource VulkanDeferredRender::createViewMatrixUniformBufferResource(
            const UINT setIndex,
            const UINT viewMatrixUboIndex)
        {
            VulkanUniformBufferResource viewMatrixUniformBufferResource = {};

            m_descriptorUpdateDataTable[setIndex].push_back(
                { viewMatrixUboIndex,
                  const_cast<Math::Mat4*>(&(m_pScene->GetCamera(0)->GetViewMatrix())) });

            Buffer::VulkanUniformBuffer* pViewMatUniformBuffer = new Buffer::VulkanUniformBuffer(m_pDevice);
            pViewMatUniformBuffer->createUniformBuffer(
                *m_pHwDevice, 1, sizeof(Math::Mat4), static_cast<const void*>(&(m_pScene->GetCamera(0)->GetViewMatrix())));

            const size_t descriptorBufferIndex = m_pDescriptorBufferList.size();
            m_pDescriptorBufferList.push_back(
                std::unique_ptr<Buffer::VulkanDescriptorBuffer>(pViewMatUniformBuffer));

            viewMatrixUniformBufferResource.shaderType       = BX_FRAGMENT_SHADER;
            viewMatrixUniformBufferResource.setIndex         = setIndex;
            viewMatrixUniformBufferResource.bindingPoint     = viewMatrixUboIndex;
            viewMatrixUniformBufferResource.uniformbufferNum = 1;
            viewMatrixUniformBufferResource.pUniformBuffer   =
                static_cast<Buffer::VulkanUniformBuffer*>(m_pDescriptorBufferList[descriptorBufferIndex].get());

            return viewMatrixUniformBufferResource;
        }

        std::vector<VulkanUniformBufferResource> VulkanDeferredRender::createGbufferUniformBufferResource()
        {
            std::vector<VulkanUniformBufferResource> gBufferUniformbufferResourceList =
            {
                createTransMatrixUniformBufferResource(0, TRANSFORM_MATRIX_UBO_INDEX),
                createMaterialUniformBufferResource(0, MATERIAL_UBO_INDEX)
            };

            return gBufferUniformbufferResourceList;
        }

        VulkanUniformBufferResource VulkanDeferredRender::createMsaaUniformBufferResource(
            const UINT setIndex,
            const UINT msaaUboIndex)
        {
            VulkanUniformBufferResource msaaUniformBufferResource = {};

            m_msaaUniformBuffer.sampleNum = m_pSetting->m_graphicsSetting.antialasing;
            m_msaaUniformBuffer.dimension = m_pSetting->resolution;

            m_descriptorUpdateDataTable[setIndex].push_back({ msaaUboIndex, static_cast<void*>(&m_msaaUniformBuffer) });

            Buffer::VulkanUniformBuffer* pMsaaUniformBuffer = new Buffer::VulkanUniformBuffer(m_pDevice);
            pMsaaUniformBuffer->createUniformBuffer(
                *m_pHwDevice, 1, sizeof(MsaaUniformBuffer), static_cast<void*>(&m_msaaUniformBuffer));

            const size_t descriptorBufferIndex = m_pDescriptorBufferList.size();
            m_pDescriptorBufferList.push_back(
                std::unique_ptr<Buffer::VulkanDescriptorBuffer>(pMsaaUniformBuffer));

            msaaUniformBufferResource.shaderType       = BX_FRAGMENT_SHADER;
            msaaUniformBufferResource.setIndex         = setIndex;
            msaaUniformBufferResource.bindingPoint     = msaaUboIndex;
            msaaUniformBufferResource.uniformbufferNum = 1;
            msaaUniformBufferResource.pUniformBuffer   =
                static_cast<Buffer::VulkanUniformBuffer*>(m_pDescriptorBufferList[descriptorBufferIndex].get());

            return msaaUniformBufferResource;
        }

        std::vector<VulkanUniformBufferResource> VulkanDeferredRender::createShadingPassUniformBufferResource()
        {
            std::vector<VulkanUniformBufferResource> shadingPassUniformbufferResourceList =
            {
                createLightUniformBufferResource(1, LIGHT_UBO_INDEX),
                createCamUniformBufferResource(1, CAM_UBO_INDEX),
                createViewMatrixUniformBufferResource(1, VIEW_MATRIX_UBO_INDEX)
            };

            if (m_pSetting->m_graphicsSetting.IsEnableAntialasing() == TRUE)
            {
                shadingPassUniformbufferResourceList.push_back(createMsaaUniformBufferResource(1, MSAA_UBO_INDEX));
            }

            return shadingPassUniformbufferResourceList;
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
                pRTCreateData->attachmentIndex              = static_cast<UINT>(RTIndex);
                pRTCreateData->sampleNum                    = pRTDesc->sampleNum;
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
            IN  const VulkanRenderProperties&                              renderProps,
            IN  std::vector<VulkanRenderTargetCreateData*>*                pRTCreateDataRefList,
            OUT Shader::BxShaderMeta*                                      pGBufferShaderMeta,
            OUT std::vector<VulkanUniformBufferResource>*                  pGBufferUniformBufferResourceList,
            OUT std::vector<VulkanTextureResource>*                        pGBufferPassTextureResourceList,
            OUT std::vector<VulkanDescriptorResources>*                    pGBufferDescriptorResourcesList,
            OUT VulkanRenderResources*                                     pGBufferResources,
            OUT std::vector<VulkanGraphicsPipelineRenderTargetProperties>* pGBufferGraphicsPipelineRTProperties,
            OUT VkVertexInputBindingDescription*                           pGBufferVertexInputBindingDescription,
            OUT std::vector<VkVertexInputAttributeDescription>*            pGBufferVertexInputAttributeDescription,
            OUT VulkanGraphicsPipelineProperties*                          pGBufferGraphicsPipelineProperties,
            OUT VulkanSubpassGraphicsPipelineCreateData*                   pGBufferGraphicsPipelineCreateData)
        {
            VulkanRenderSubpassCreateData gBufferPassCreateData = {};

            const UINT gBufferPassIndex = 0;

            //  GBuffer shaders
            pGBufferShaderMeta->vertexShaderInfo   = { "GBuffer.vert.spv", "main" };
            pGBufferShaderMeta->fragmentShaderInfo = { "GBuffer.frag.spv", "main" };

            pGBufferResources->vertexDescriptionBindingPoint = 0;
            pGBufferResources->pVertexInputResourceList      = &m_mainSceneVertexInputResourceList;

            VulkanDescriptorResources* pGBufferDescriptorResources =
                &(pGBufferDescriptorResourcesList->at(TRANSFORM_MATRIX_UBO_INDEX));

            pGBufferDescriptorResources->descriptorSetIndex = 0;

            // Initialize uniform buffers for render pass
            pGBufferDescriptorResources->pUniformBufferResourceList = pGBufferUniformBufferResourceList;

            // Initialize albedo textures for gbuffer pass
            ::Texture::TextureSamplerCreateData textureSamplerCreateData = {};
            textureSamplerCreateData.minFilter       = BX_TEXTURE_SAMPLER_FILTER_LINEAR;
            textureSamplerCreateData.magFilter       = BX_TEXTURE_SAMPLER_FILTER_LINEAR;
            textureSamplerCreateData.addressingModeU = BX_TEXTURE_SAMPLER_ADDRESSING_CLAMP_TO_BORDER;
            textureSamplerCreateData.addressingModeV = BX_TEXTURE_SAMPLER_ADDRESSING_CLAMP_TO_BORDER;
            textureSamplerCreateData.anisotropyNum   = static_cast<float>(m_pSetting->m_graphicsSetting.anisotropy);
            textureSamplerCreateData.borderColor     = { 0.0f, 0.0f, 0.0f, 0.0f };
            textureSamplerCreateData.normalize       = TRUE;
            textureSamplerCreateData.mipmapFilter    = BX_TEXTURE_SAMPLER_FILTER_LINEAR;

            Texture::VulkanTexture2D* pDefaultAlbedo =
                m_pTextureMgr->createTexture2DSampler("../resources/textures/teaport/wall.jpg",
                                                      1,
                                                      TRUE,
                                                      BX_FORMAT_RGBA8,
                                                      BX_FORMAT_RGBA8,
                                                      textureSamplerCreateData);

            const size_t sceneAlbedoMaterialNum = m_mainSceneMeshMaterialMapResourceList.size();
            std::vector<Texture::VulkanTexture2D*> sceneAlbedoMapPtrList(sceneAlbedoMaterialNum);

            for (size_t albedoMaterialIndex = 0;
                 albedoMaterialIndex < sceneAlbedoMaterialNum;
                 albedoMaterialIndex++)
            {
                const Texture::VulkanTexture2D* pAlbedoMap =
                    m_mainSceneMeshMaterialMapResourceList[albedoMaterialIndex].pDiffuseMap;

                if (pAlbedoMap != NULL)
                {
                    sceneAlbedoMapPtrList[albedoMaterialIndex] =
                        m_mainSceneMeshMaterialMapResourceList[albedoMaterialIndex].pDiffuseMap;
                }
                else
                {
                    sceneAlbedoMapPtrList[albedoMaterialIndex] = pDefaultAlbedo;
                }
            }

            pGBufferPassTextureResourceList->push_back(
                createSceneTextures(gBufferPassIndex, ALBEDO_TEXTURE_INDEX, sceneAlbedoMapPtrList));

            pGBufferDescriptorResources->pTextureResouceList = pGBufferPassTextureResourceList;

            pGBufferResources->pDescriptorResourceList = pGBufferDescriptorResourcesList;

            /// Create gbuffer graphics pipeline
            const UINT gBufferColorRenderTargetNum =
                static_cast<const UINT>(pGBufferGraphicsPipelineRTProperties->size());

            for (UINT colorRTIndex = 0; colorRTIndex < gBufferColorRenderTargetNum; ++colorRTIndex)
            {
                pGBufferGraphicsPipelineRTProperties->at(colorRTIndex).enableBlend = m_pSetting->m_graphicsSetting.blend;
            }

            *pGBufferVertexInputBindingDescription =
                Buffer::VulkanVertexBuffer::CreateDescription(0, sizeof(Object::Model::Vertex), BX_VERTEX_INPUT_RATE_VERTEX);

            *pGBufferVertexInputAttributeDescription =
                Buffer::VulkanVertexBuffer::CreateAttributeDescriptions(
                    Buffer::VulkanVertexBuffer::GenSingleTextureChannelMeshAttributeListCreateData(0));

            pGBufferGraphicsPipelineProperties->cullMode            = CULLMODE_BACK;
            pGBufferGraphicsPipelineProperties->polyMode            = POLYMODE_FILL;
            pGBufferGraphicsPipelineProperties->samples             = m_pSetting->m_graphicsSetting.antialasing;
            pGBufferGraphicsPipelineProperties->viewportRects       = { renderProps.renderViewportRect };
            pGBufferGraphicsPipelineProperties->scissorRects        = { renderProps.renderViewportRect };
            pGBufferGraphicsPipelineProperties->pRenderTargetsProps = pGBufferGraphicsPipelineRTProperties;

            pGBufferGraphicsPipelineCreateData->subpassIndex                         = 0;
            pGBufferGraphicsPipelineCreateData->pProps                               = pGBufferGraphicsPipelineProperties;
            pGBufferGraphicsPipelineCreateData->pShaderMeta                          = pGBufferShaderMeta;
            pGBufferGraphicsPipelineCreateData->pResource                            = pGBufferResources;
            pGBufferGraphicsPipelineCreateData->isScenePipeline                      = TRUE;
            pGBufferGraphicsPipelineCreateData->pVertexInputBindingDescription       = pGBufferVertexInputBindingDescription;
            pGBufferGraphicsPipelineCreateData->pVertexInputAttributeDescriptionList = pGBufferVertexInputAttributeDescription;

            // Generate subpass graphics pipeline create data
            gBufferPassCreateData.pSubpassGraphicsPipelineCreateData    = pGBufferGraphicsPipelineCreateData;
            gBufferPassCreateData.pSubpassRenderTargetCreateDataRefList = pRTCreateDataRefList;

            return gBufferPassCreateData;
        }

        VulkanRenderSubpassCreateData VulkanDeferredRender::genShadingPassCreateData(
            IN const VulkanRenderProperties&                               renderProps,
            IN  std::vector<VulkanRenderTargetCreateData*>*                pRTCreateDataRefList,
            OUT Shader::BxShaderMeta*                                      pShadingPassShaderMeta,
            OUT std::vector<VulkanUniformBufferResource>*                  pShadingPassUniformBufferResourceList,
            OUT std::vector<VulkanTextureResource>*                        pShadingPassTextureResourceList,
            OUT std::vector<VulkanTextureResource>*                        pInputAttachmentResourceList,
            OUT std::vector<VulkanDescriptorResources>*                    pShadingPassDescriptorResourcesList,
            OUT VulkanRenderResources*                                     pShadingPassResources,
            OUT std::vector<VulkanGraphicsPipelineRenderTargetProperties>* pShadingPassGraphicsPipelineRTProperties,
            OUT VkVertexInputBindingDescription*                           pShadingPassVertexInputBindingDescription,
            OUT std::vector<VkVertexInputAttributeDescription>*            pShadingPassVertexInputAttributeDescription,
            OUT VulkanGraphicsPipelineProperties*                          pShadingPassGraphicsPipelineProps,
            OUT VulkanSubpassGraphicsPipelineCreateData*                   pShadingPassGraphicsPipelineCreateData)
        {
            VulkanRenderSubpassCreateData shadingPassCreateData = {};

            const UINT shadingPassIndex = 1;
            const UINT RTNum            = GBUFFER_NUM - 1;
            const UINT backbufferNum    = static_cast<const UINT>(m_backBufferRTsCreateDataList.size());

            // Backbuffer shader
            if (m_pSetting->m_graphicsSetting.IsEnableAntialasing() == FALSE)
            {
                pShadingPassShaderMeta->vertexShaderInfo   = { "MainSceneDeferred.vert.spv", "main" };
                pShadingPassShaderMeta->fragmentShaderInfo = { "MainSceneDeferred.frag.spv", "main" };
            }
            else
            {
                pShadingPassShaderMeta->vertexShaderInfo   = { "MainSceneDeferredMS.vert.spv", "main" };
                pShadingPassShaderMeta->fragmentShaderInfo = { "MainSceneDeferredMS.frag.spv", "main" };
            }

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

            pShadingPassResources->vertexDescriptionBindingPoint = 0;
            pShadingPassResources->pVertexInputResourceList      = &m_shadingPassVertexInput;

            // Generate shading pass descriptor create data
            VulkanDescriptorResources* pShadingPassDescriptorResources = &(pShadingPassDescriptorResourcesList->at(0));

            pShadingPassDescriptorResources->descriptorSetIndex = 1;

            pShadingPassDescriptorResources->pUniformBufferResourceList = pShadingPassUniformBufferResourceList;

            pShadingPassResources->pDescriptorResourceList = pShadingPassDescriptorResourcesList;

            // Create input attachment resources
            if (m_pSetting->m_graphicsSetting.IsEnableAntialasing() == FALSE)
            {
                pInputAttachmentResourceList->resize(GBUFFER_NUM - 1);

                VulkanTextureResource* pPosTextureInputAttachment       = &(pInputAttachmentResourceList->at(GBUFFER_POS_BUFFER_INDEX - 1));
                pPosTextureInputAttachment->bindingPoint                = GBUFFER_POS_TEX_INDEX;
                pPosTextureInputAttachment->attachmentIndex             = 1;
                pPosTextureInputAttachment->setIndex                    = shadingPassIndex;
                pPosTextureInputAttachment->shaderType                  = BX_FRAGMENT_SHADER;
                pPosTextureInputAttachment->pTextureList                =
                    { m_backBufferRTsCreateDataList[0][GBUFFER_POS_BUFFER_INDEX].attachment.pTex };

                VulkanTextureResource* pNormalTextureInputAttachment    = &(pInputAttachmentResourceList->at(GBUFFER_NORMAL_BUFFER_INDEX - 1));
                pNormalTextureInputAttachment->bindingPoint             = GBUFFER_NORMAL_TEX_INDEX;
                pNormalTextureInputAttachment->attachmentIndex          = 2;
                pNormalTextureInputAttachment->setIndex                 = shadingPassIndex;
                pNormalTextureInputAttachment->shaderType               = BX_FRAGMENT_SHADER;
                pNormalTextureInputAttachment->pTextureList             =
                    { m_backBufferRTsCreateDataList[0][GBUFFER_NORMAL_BUFFER_INDEX].attachment.pTex };

                VulkanTextureResource* pTexCoord0TextureInputAttachment = &(pInputAttachmentResourceList->at(GBUFFER_ALBEDO_BUFFER_INDEX - 1));
                pTexCoord0TextureInputAttachment->bindingPoint          = GBUFFER_ALBEDO_TEX_INDEX;
                pTexCoord0TextureInputAttachment->attachmentIndex       = 3;
                pTexCoord0TextureInputAttachment->setIndex              = shadingPassIndex;
                pTexCoord0TextureInputAttachment->shaderType            = BX_FRAGMENT_SHADER;
                pTexCoord0TextureInputAttachment->pTextureList          =
                    { m_backBufferRTsCreateDataList[0][GBUFFER_ALBEDO_BUFFER_INDEX].attachment.pTex };

                pShadingPassDescriptorResources->pInputAttachmentList = pInputAttachmentResourceList;
            }
            else
            {
                // Create GBuffer resources
                ::Texture::TextureSamplerCreateData gBufferSamplerCreateData = {};
                gBufferSamplerCreateData.minFilter                           = BX_TEXTURE_SAMPLER_FILTER_LINEAR;
                gBufferSamplerCreateData.magFilter                           = BX_TEXTURE_SAMPLER_FILTER_LINEAR;
                gBufferSamplerCreateData.addressingModeU                     = BX_TEXTURE_SAMPLER_ADDRESSING_CLAMP_TO_BORDER;
                gBufferSamplerCreateData.addressingModeV                     = BX_TEXTURE_SAMPLER_ADDRESSING_CLAMP_TO_BORDER;
                gBufferSamplerCreateData.anisotropyNum                       = static_cast<float>(m_pSetting->m_graphicsSetting.anisotropy);
                gBufferSamplerCreateData.borderColor                         = { 0.0f, 0.0f, 0.0f, 0.0f };
                gBufferSamplerCreateData.normalize                           = TRUE;
                gBufferSamplerCreateData.mipmapFilter                        = BX_TEXTURE_SAMPLER_FILTER_LINEAR;

                 Texture::VulkanTexture2D* pPosTexture       =
                    static_cast<Texture::VulkanTexture2D*>(m_backBufferRTsCreateDataList[0][GBUFFER_POS_BUFFER_INDEX].attachment.pTex);
                pPosTexture->createSampler(&gBufferSamplerCreateData, m_pTextureMgr->IsAnisotropySupport());

                 Texture::VulkanTexture2D* pNormalTexture    =
                    static_cast<Texture::VulkanTexture2D*>(m_backBufferRTsCreateDataList[0][GBUFFER_NORMAL_BUFFER_INDEX].attachment.pTex);
                pNormalTexture->createSampler(&gBufferSamplerCreateData, m_pTextureMgr->IsAnisotropySupport());

                 Texture::VulkanTexture2D* pTexCoord0Texture =
                    static_cast<Texture::VulkanTexture2D*>(m_backBufferRTsCreateDataList[0][GBUFFER_ALBEDO_BUFFER_INDEX].attachment.pTex);
                pTexCoord0Texture->createSampler(&gBufferSamplerCreateData, m_pTextureMgr->IsAnisotropySupport());

                pShadingPassTextureResourceList->push_back(
                    createSceneTextures(shadingPassIndex, 0, { pPosTexture }));

                pShadingPassTextureResourceList->push_back(
                    createSceneTextures(shadingPassIndex, 1, { pNormalTexture }));

                pShadingPassTextureResourceList->push_back(
                    createSceneTextures(shadingPassIndex, 2, { pTexCoord0Texture }));

                pShadingPassDescriptorResources->pTextureResouceList = pShadingPassTextureResourceList;
            }

            /// Create shading pass graphics pipeline
            const UINT shadingPassColorRenderTargetNum =
                static_cast<const UINT>(pShadingPassGraphicsPipelineRTProperties->size());

            for (UINT colorRTIndex = 0; colorRTIndex < shadingPassColorRenderTargetNum; ++colorRTIndex)
            {
                pShadingPassGraphicsPipelineRTProperties->at(colorRTIndex).enableBlend = m_pSetting->m_graphicsSetting.blend;
            }

            *pShadingPassVertexInputBindingDescription =
                Buffer::VulkanVertexBuffer::CreateDescription(0, sizeof(Math::Vector2), BX_VERTEX_INPUT_RATE_VERTEX);

            *pShadingPassVertexInputAttributeDescription =
                Buffer::VulkanVertexBuffer::CreateAttributeDescriptions(
                    Buffer::VulkanVertexBuffer::GenPosQuadAttributeListCreateData(0));

            pShadingPassGraphicsPipelineProps->cullMode            = CULLMODE_BACK;
            pShadingPassGraphicsPipelineProps->polyMode            = POLYMODE_FILL;
            pShadingPassGraphicsPipelineProps->samples             = 1;
            pShadingPassGraphicsPipelineProps->viewportRects       = { renderProps.renderViewportRect };
            pShadingPassGraphicsPipelineProps->scissorRects        = { renderProps.renderViewportRect };
            pShadingPassGraphicsPipelineProps->pRenderTargetsProps = pShadingPassGraphicsPipelineRTProperties;

            pShadingPassGraphicsPipelineCreateData->subpassIndex                         = 1;
            pShadingPassGraphicsPipelineCreateData->pProps                               = pShadingPassGraphicsPipelineProps;
            pShadingPassGraphicsPipelineCreateData->pShaderMeta                          = pShadingPassShaderMeta;
            pShadingPassGraphicsPipelineCreateData->pResource                            = pShadingPassResources;
            pShadingPassGraphicsPipelineCreateData->isScenePipeline                      = FALSE;
            pShadingPassGraphicsPipelineCreateData->pVertexInputBindingDescription       = pShadingPassVertexInputBindingDescription;
            pShadingPassGraphicsPipelineCreateData->pVertexInputAttributeDescriptionList = pShadingPassVertexInputAttributeDescription;

            /// Generate shading pass create data
            shadingPassCreateData.pSubpassGraphicsPipelineCreateData    = pShadingPassGraphicsPipelineCreateData;
            shadingPassCreateData.pSubpassRenderTargetCreateDataRefList = pRTCreateDataRefList;

            return shadingPassCreateData;
        }
    }
}