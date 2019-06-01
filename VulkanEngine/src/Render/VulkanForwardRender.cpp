//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include <Object/Model/Mesh/Mesh.h>

#include "VulkanRender.h"

#define BACK_BUFFER_INDEX               0
#define BACK_BUFFER_DEPTH_BUFFER_INDEX  1

// Descriptor binding points
#define TRANSFORM_MATRIX_UBO_INDEX      0
#define MATERIAL_UBO_INDEX              1
#define LIGHT_UBO_INDEX                 2
#define STATIC_UBO_INDEX                3
#define ALBEDO_TEXTURE_INDEX            4
#define SPECULAR_TEXTURE_INDEX          5

#define DESCRIPTOR_SET_NUM              2

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
            m_descriptorSetNum = DESCRIPTOR_SET_NUM;

            if (m_pSetting->m_graphicsSetting.shadowCasting == TRUE)
            {
                m_descriptorSetNum++;
            }

            m_descriptorUpdateDataTable.resize(m_descriptorSetNum);
        }

        VulkanForwardRender::~VulkanForwardRender()
        {
        }

        BOOL VulkanForwardRender::initialize()
        {
            BOOL status = BX_SUCCESS;

            // Initialize backbuffer render targets
            initializeBackbufferRTCreateData();

            /// Initialize all render passes which need to be used in forward render
            if (m_pSetting->m_graphicsSetting.shadowCasting == TRUE)
            {
                status = createShadowPass();
            }

            status = createMainSceneRenderPass();

            assert(status == BX_SUCCESS);

            return status;
        }

        BOOL VulkanForwardRender::update(
            const float deltaTime)
        {
            BOOL status = BX_SUCCESS;

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

            for (VulkanRenderPass& preRenderPass : m_preDrawPassList)
            {
                status = preRenderPass.update(deltaTime, m_descriptorUpdateDataTable);

                assert(status == BX_SUCCESS);
            }

            status = m_mainSceneRenderPass.update(deltaTime, m_descriptorUpdateDataTable);
            assert(status == BX_SUCCESS);

            for (VulkanRenderPass& postRenderPass : m_postDrawPassList)
            {
                // status = postRenderPass.update(deltaTime);

                assert(status == BX_SUCCESS);
            }

            return status;
        }

        void VulkanForwardRender::draw()
        {
            BOOL status = BX_SUCCESS;

            UINT preDrawNum = static_cast<UINT>(m_preDrawPassList.size());

            for (UINT preDrawPassIndex = 0; preDrawPassIndex < preDrawNum; ++preDrawPassIndex)
            {
                status = m_preDrawPassList[preDrawPassIndex].draw();

                assert(status == BX_SUCCESS);
            }

            m_mainSceneRenderPass.draw();

            assert(status == BX_SUCCESS);

            UINT postDrawNum = static_cast<UINT>(m_postDrawPassList.size());
            for (UINT postDrawPassIndex = 0; postDrawPassIndex < postDrawNum; ++postDrawPassIndex)
            {
                status = m_postDrawPassList[postDrawPassIndex].draw();

                assert(status == BX_SUCCESS);
            }
        }

        BOOL VulkanForwardRender::createMainSceneRenderPass()
        {
            BOOL status = BX_SUCCESS;

            /// Initialize default render pass for main scene
            //  Initialize graphics pipeline properties
            VulkanRenderProperties props = {};
            props.enableBlending         = TRUE;
            props.enableColor            = TRUE;

            if (m_pSetting->m_graphicsSetting.antialasing == AA_NONE)
            {
                props.sceneClearValue = { { 0.0f, 0.0f, 0.0f, 1.0f } };
            }
            else
            {
                props.sceneClearValue = { { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f } };
            }

            props.renderViewportRect =
            {
                0.0f, static_cast<float>(m_pSetting->resolution.width),
                static_cast<float>(m_pSetting->resolution.height), 0.0f
            };

            // Initialize shaders
            Shader::BxShaderMeta mainSceneShaderMeta        = {};
            mainSceneShaderMeta.vertexShaderInfo.shaderFile = "MainSceneForward.vert.spv";

            if (m_pSetting->m_graphicsSetting.shadingMethod == ShadingMethod::Phong)
            {
                mainSceneShaderMeta.fragmentShaderInfo.shaderFile = "MainSceneForward.frag.spv";
            }
            else if (m_pSetting->m_graphicsSetting.shadingMethod == ShadingMethod::PBR_COOK_TORRANCE)
            {
                mainSceneShaderMeta.fragmentShaderInfo.shaderFile = "MainSceneForwardPBR.frag.spv";
            }

            /// Initialize render pass
            const UINT sampleNum = m_pSetting->m_graphicsSetting.antialasing;

            std::vector<VulkanRenderTargetCreateDescriptor> renderTargetDescriptors =
            {
                { TRUE, 0, 0, 1, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_PRESENT, FALSE, FALSE }
            };

            if (m_pSetting->m_graphicsSetting.antialasing != AA_NONE)
            {
                renderTargetDescriptors.push_back(
                    { TRUE, 0, 2, sampleNum, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_COLOR, FALSE, FALSE });
            }

            assert((IsDepthTestEnabled() == TRUE)  ||
                   ((IsDepthTestEnabled()   == FALSE) &&
                    (IsStencilTestEnabled() == FALSE)));

            // Create depth buffer
            if (IsDepthTestEnabled() == TRUE)
            {
                props.enableDepth     = TRUE;
                props.depthClearValue = { 1.0f, 0.0f };

                genBackbufferDepthBuffer();

                renderTargetDescriptors.push_back({ TRUE, 0, 1, sampleNum, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_DEPTH_STENCIL, FALSE, FALSE });
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
                pRenderTargetCreateData->sampleNum          = renderTargetDescriptors[attachmentIndex].sampleNum;
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

            // Initialize vertex input for render pass
            VulkanRenderResources renderSources         = {};
            renderSources.vertexDescriptionBindingPoint = 0;
            renderSources.pVertexInputResourceList      = &m_mainSceneVertexInputResourceList;

            std::vector<VulkanDescriptorResources> descriptorResourceList;

            VulkanDescriptorResources descriptorResources = {};
            descriptorResources.descriptorSetIndex        = 0;

            // Initialize uniform buffers for render pass
            std::vector<VulkanUniformBufferResource> uniformBufferResourceList = createUniformBufferResource();
            descriptorResources.pUniformBufferResourceList                     = &uniformBufferResourceList;

            // Initialize albedo material maps

            // Initialize default albedo material
            ::Texture::TextureSamplerCreateData textureSamplerCreateData = {};
            textureSamplerCreateData.minFilter       = BX_TEXTURE_SAMPLER_FILTER_LINEAR;
            textureSamplerCreateData.magFilter       = BX_TEXTURE_SAMPLER_FILTER_LINEAR;
            textureSamplerCreateData.addressingModeU = BX_TEXTURE_SAMPLER_ADDRESSING_CLAMP_TO_EDGE;
            textureSamplerCreateData.addressingModeV = BX_TEXTURE_SAMPLER_ADDRESSING_CLAMP_TO_EDGE;
            textureSamplerCreateData.anisotropyNum   = static_cast<float>(m_pSetting->m_graphicsSetting.anisotropy);
            textureSamplerCreateData.borderColor     = { 0.0f, 0.0f, 0.0f, 1.0f };
            textureSamplerCreateData.normalize       = TRUE;
            textureSamplerCreateData.mipmapFilter    = BX_TEXTURE_SAMPLER_FILTER_LINEAR;

            Texture::VulkanTexture2D* pDefaultMaterialTexMap =
                m_pTextureMgr->createTexture2DSampler("../resources/textures/teaport/wall.jpg",
                                                      1,
                                                      TRUE,
                                                      BX_FORMAT_RGBA8,
                                                      BX_FORMAT_RGBA8,
                                                      textureSamplerCreateData);

            const size_t sceneMaterialNum = m_mainSceneMeshMaterialMapResourceList.size();
            std::vector<Texture::VulkanTexture2D*> sceneAlbedoMapPtrList(sceneMaterialNum);
            std::vector<Texture::VulkanTexture2D*> sceneSpecularMapPtrList(sceneMaterialNum);

            for (size_t materialIndex = 0;
                 materialIndex < sceneMaterialNum;
                 materialIndex++)
            {
                VulkanMeshMaterialMapResource* pMaterialMapResource =
                    &(m_mainSceneMeshMaterialMapResourceList[materialIndex]);

                // Diffuse
                const Texture::VulkanTexture2D* pAlbedoMap = pMaterialMapResource->pDiffuseMap;

                if (pAlbedoMap != NULL)
                {
                    sceneAlbedoMapPtrList[materialIndex] = pMaterialMapResource->pDiffuseMap;
                }
                else
                {
                    sceneAlbedoMapPtrList[materialIndex] = pDefaultMaterialTexMap;
                }

                // Specular
                const Texture::VulkanTexture2D* pSpecularMap = pMaterialMapResource->pSpecularMap;

                if (pSpecularMap != NULL)
                {
                    sceneSpecularMapPtrList[materialIndex] = pMaterialMapResource->pSpecularMap;
                }
                else
                {
                    sceneSpecularMapPtrList[materialIndex] = pDefaultMaterialTexMap;
                }
            }

            std::vector<VulkanTextureResource> sceneTextureResourceList =
            {
                createSceneTextures(0, ALBEDO_TEXTURE_INDEX, sceneAlbedoMapPtrList),
                createSceneTextures(0, SPECULAR_TEXTURE_INDEX, sceneSpecularMapPtrList)
            };

            descriptorResources.pTextureResouceList = &sceneTextureResourceList;

            // Create resolve resources for MSAA
            std::vector<VulkanTextureResource> resolveTextureResourceList(backbufferNum);
            if (m_pSetting->m_graphicsSetting.antialasing != Antialasing::AA_NONE)
            {
                for (UINT backbufferIndex = 0; backbufferIndex < backbufferNum; ++backbufferIndex)
                {
                    VulkanTextureResource* resolveTextureResource = &(resolveTextureResourceList[backbufferIndex]);
                    resolveTextureResource->setIndex              = 0;
                    resolveTextureResource->attachmentIndex       = 0;

                    resolveTextureResource->pTextureList.push_back(
                        m_backBufferRTsCreateDataList[backbufferIndex][0].attachment.pTex);
                }

                descriptorResources.pResolveAttachmentList = &resolveTextureResourceList;
            }

            // Added descriptor resources to the resource list
            descriptorResourceList.push_back(descriptorResources);

            renderSources.pDescriptorResourceList = &descriptorResourceList;

            // Create render pass create data
            std::vector<VulkanGraphicsPipelineRenderTargetProperties> renderTargetsProps(1);
            renderTargetsProps[0].enableBlend = m_pSetting->m_graphicsSetting.blend;

            VkVertexInputBindingDescription vertexInputBindingDescription =
                Buffer::VulkanVertexBuffer::CreateDescription(0, sizeof(Object::Model::Vertex), BX_VERTEX_INPUT_RATE_VERTEX);

            std::vector<VkVertexInputAttributeDescription> vertexAttributeDescription =
                Buffer::VulkanVertexBuffer::CreateAttributeDescriptions(
                    Buffer::VulkanVertexBuffer::GenSingleTextureChannelMeshAttributeListCreateData(0));

            VulkanGraphicsPipelineProperties subpassGraphicsPipelineProperties     = {};
            subpassGraphicsPipelineProperties.cullMode                             = CULLMODE_BACK;
            subpassGraphicsPipelineProperties.polyMode                             = POLYMODE_FILL;
            subpassGraphicsPipelineProperties.samples                              = m_pSetting->m_graphicsSetting.antialasing;
            subpassGraphicsPipelineProperties.viewportRects                        = { props.renderViewportRect };
            subpassGraphicsPipelineProperties.scissorRects                         = { props.renderViewportRect };
            subpassGraphicsPipelineProperties.pRenderTargetsProps                  = &renderTargetsProps;

            VulkanSubpassGraphicsPipelineCreateData subpassGraphicsPipelineCreateData = {};
            subpassGraphicsPipelineCreateData.subpassIndex                            = 0;
            subpassGraphicsPipelineCreateData.pProps                                  = &subpassGraphicsPipelineProperties;
            subpassGraphicsPipelineCreateData.pShaderMeta                             = &mainSceneShaderMeta;
            subpassGraphicsPipelineCreateData.pResource                               = &renderSources;
            subpassGraphicsPipelineCreateData.isScenePipeline                         = TRUE;
            subpassGraphicsPipelineCreateData.pVertexInputBindingDescription          = &vertexInputBindingDescription;
            subpassGraphicsPipelineCreateData.pVertexInputAttributeDescriptionList    = &vertexAttributeDescription;

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

            return status;
        }

        void VulkanForwardRender::initializeBackbufferRTCreateData()
        {
            UINT backBufferTextureSize = 1;

            if (m_pSetting->m_graphicsSetting.antialasing != AA_NONE)
            {
                backBufferTextureSize++;
            }

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

                if (m_pSetting->m_graphicsSetting.antialasing != Antialasing::AA_NONE)
                {
                    m_backBufferRTsCreateDataList[backBufferIndex][BACK_BUFFER_INDEX + 1] =
                    {
                        static_cast<UINT>(backBufferIndex),
                        { pBackbufferTexture->GetSampleNumber(), pBackbufferTexture }
                    };
                }
            }
        }

        std::vector<VulkanUniformBufferResource> VulkanForwardRender::createUniformBufferResource()
        {
            std::vector<VulkanUniformBufferResource> uniformbufferResourceList =
            {
                createTransMatrixUniformBufferResource(0, TRANSFORM_MATRIX_UBO_INDEX),
                createMaterialUniformBufferResource(0, MATERIAL_UBO_INDEX),
                createLightUniformBufferResource(0, LIGHT_UBO_INDEX),
                createCamUniformBufferResource(m_pScene->GetCamera(0), 0, STATIC_UBO_INDEX),
            };

            return uniformbufferResourceList;
        }
    }
}