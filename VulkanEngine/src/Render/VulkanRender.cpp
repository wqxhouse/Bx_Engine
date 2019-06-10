//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include <Texture/Texture2D.h>
#include <Object/Model/Material/Material.h>

#include "VulkanRender.h"

namespace VulkanEngine
{
    namespace Render
    {
        VulkanRenderBase::VulkanRenderBase(
            const Setting*                                const pSetting,
            const VkPhysicalDevice*                       const pHwDevice,
            const VkDevice*                               const pDevice,
            Mgr::CmdBufferMgr*                            const pCmdBufferMgr,
            Mgr::DescriptorMgr*                           const pDescritorMgr,
            Mgr::TextureMgr*                              const pTextureMgr,
            const Scene::RenderScene*                           pScene,
            const std::vector<Texture::VulkanTexture2D*>* const ppBackbufferTextures)
            : m_pSetting(pSetting),
              m_pHwDevice(pHwDevice),
              m_pDevice(pDevice),
              m_pCmdBufferMgr(pCmdBufferMgr),
              m_pDescriptorMgr(pDescritorMgr),
              m_pTextureMgr(pTextureMgr),
              m_pScene(pScene),
              m_renderPassNum(1),
              m_mainSceneRenderPass(pSetting, pDevice, pCmdBufferMgr, pDescritorMgr, pScene, 0, MAIN_RENDER_PASS),
              m_isDepthTestEnabled(FALSE),
              m_isStencilTestEnabled(FALSE),
              m_ppBackbufferTextures(ppBackbufferTextures),
              m_backBufferRTsCreateDataList(ppBackbufferTextures->size()),
              m_materialUniformBuffer(DEFAULT_MAX_RENDER_SCENE_OBJ_NUM)
        {
            assert(ppBackbufferTextures->size() > 0);

            parseScene();
        }

        VulkanRenderBase::~VulkanRenderBase()
        {
        }

        void VulkanRenderBase::parseScene()
        {
            // Parse Models
            UINT meshId = 0;

            const UINT modelNum = m_pScene->GetSceneModelNum();

            for (UINT modelIndex = 0; modelIndex < modelNum; ++modelIndex)
            {
                const Object::Model::ModelObject* pModel = m_pScene->GetModel(modelIndex);

                if (pModel->IsEnable())
                {
                    const UINT meshNum = pModel->GetMeshNum();

                    for (UINT meshIndex = 0; meshIndex < meshNum; ++meshIndex)
                    {
                        const std::shared_ptr<Object::Model::Mesh> pMesh = pModel->GetMesh(meshIndex);

                        if (pMesh->m_vertexBuffer.size() > 0)
                        {
                            Buffer::VulkanVertexBuffer* pVertexBuffer =
                                new Buffer::VulkanVertexBuffer(m_pDevice,
                                                               m_pCmdBufferMgr,
                                                               pMesh);

                            pVertexBuffer->createVertexBuffer(*m_pHwDevice, TRUE);

                            Buffer::VulkanIndexBuffer* pIndexBuffer =
                                new Buffer::VulkanIndexBuffer(m_pDevice,
                                                              m_pCmdBufferMgr,
                                                              pMesh);

                            pIndexBuffer->createIndexBuffer(*m_pHwDevice, TRUE);

                            m_mainSceneVertexInputResourceList.push_back(
                            {
                                std::unique_ptr<Buffer::VulkanVertexBuffer>(pVertexBuffer),
                                std::unique_ptr<Buffer::VulkanIndexBuffer>(pIndexBuffer)
                            });

                            /// Parse the mesh materials
                            ::Texture::TextureSamplerCreateData textureSamplerCreateData = {};
                            textureSamplerCreateData.minFilter                           = BX_TEXTURE_SAMPLER_FILTER_LINEAR;
                            textureSamplerCreateData.magFilter                           = BX_TEXTURE_SAMPLER_FILTER_LINEAR;
                            textureSamplerCreateData.addressingModeU                     = BX_TEXTURE_SAMPLER_ADDRESSING_CLAMP_TO_EDGE;
                            textureSamplerCreateData.addressingModeV                     = BX_TEXTURE_SAMPLER_ADDRESSING_CLAMP_TO_EDGE;
                            textureSamplerCreateData.anisotropyNum                       = static_cast<float>(m_pSetting->m_graphicsSetting.anisotropy);
                            textureSamplerCreateData.borderColor                         = { 0.0f, 0.0f, 0.0f, 1.0f };
                            textureSamplerCreateData.normalize                           = TRUE;
                            textureSamplerCreateData.mipmapFilter                        = BX_TEXTURE_SAMPLER_FILTER_LINEAR;

                            const Object::Model::Material*    pMaterial    = pMesh->GetMaterial();
                            const Object::Model::MaterialMap* pMaterialMap = pMesh->GetMaterialMap();

                            Texture::VulkanTexture2D* pVulkanDiffuseMap  = NULL;
                            Texture::VulkanTexture2D* pVulkanSpecularMap = NULL;
                            Texture::VulkanTexture2D* pVulkanNormalMap   = NULL;
                            Texture::VulkanTexture2D* pVulkanLightMap    = NULL;

                            // Parse diffuse map
                            if (pMaterialMap != NULL)
                            {
                                const ::Texture::Texture2D* pDiffuseMap =
                                    pMaterialMap->m_materialMapStruct.diffuseMap;

                                if (pDiffuseMap != NULL)
                                {
                                    pVulkanDiffuseMap =
                                        m_pTextureMgr->createTexture2DSampler(pDiffuseMap->GetTextureWidth(),
                                                                              pDiffuseMap->GetTextureHeight(),
                                                                              1,
                                                                              TRUE,
                                                                              BX_FORMAT_RGBA8,
                                                                              BX_FORMAT_RGBA8,
                                                                              textureSamplerCreateData,
                                                                              static_cast<const image_data*>(pDiffuseMap->GetTextureData()));
                                }
                                else
                                {
                                    UBYTE albedo[4] = { 0, 0, 0, 0 };

                                    switch (pMaterial->GetMaterialType())
                                    {
                                        case PHONG:
                                        {
                                            const Object::Model::PhongMaterial* pPhoneMaterial =
                                                static_cast<const Object::Model::PhongMaterial*>(pMaterial);

                                            albedo[0] = static_cast<UBYTE>(255.0f * pPhoneMaterial->kd[0]);
                                            albedo[1] = static_cast<UBYTE>(255.0f * pPhoneMaterial->kd[1]);
                                            albedo[2] = static_cast<UBYTE>(255.0f * pPhoneMaterial->kd[2]);
                                            albedo[3] = 255;

                                            break;
                                        }
                                        default:
                                            NotImplemented();
                                    }

                                    const size_t albedoDataSize = sizeof(image_data) * 4;

                                    image_data* fixValueDiffuseMapData =
                                        static_cast<image_data*>(malloc(albedoDataSize));

                                    memcpy(fixValueDiffuseMapData, &(albedo[0]), albedoDataSize);

                                    pVulkanDiffuseMap =
                                        m_pTextureMgr->createTexture2DSampler(1,
                                                                              1,
                                                                              1,
                                                                              TRUE,
                                                                              BX_FORMAT_RGBA8,
                                                                              BX_FORMAT_RGBA8,
                                                                              textureSamplerCreateData,
                                                                              &(fixValueDiffuseMapData[0]));
                                }
                            }

                            // Parse specular map
                            if (pMaterialMap != NULL)
                            {
                                const ::Texture::Texture2D* pSpecularMap = pMaterialMap->m_materialMapStruct.specMap;

                                if (pSpecularMap != NULL)
                                {
                                    pVulkanSpecularMap =
                                        m_pTextureMgr->createTexture2DSampler(pSpecularMap->GetTextureWidth(),
                                                                              pSpecularMap->GetTextureHeight(),
                                                                              1,
                                                                              TRUE,
                                                                              BX_FORMAT_RGBA8,
                                                                              BX_FORMAT_RGBA8,
                                                                              textureSamplerCreateData,
                                                                              static_cast<const image_data*>(pSpecularMap->GetTextureData()));
                                }
                                else
                                {
                                    UBYTE specular[4] = { 0, 0, 0, 0 };

                                    switch (pMaterial->GetMaterialType())
                                    {
                                        case PHONG:
                                        {
                                            const Object::Model::PhongMaterial* pPhoneMaterial =
                                                static_cast<const Object::Model::PhongMaterial*>(pMaterial);

                                            specular[0] = static_cast<UBYTE>(255.0f * pPhoneMaterial->ks[0]);
                                            specular[1] = static_cast<UBYTE>(255.0f * pPhoneMaterial->ks[1]);
                                            specular[2] = static_cast<UBYTE>(255.0f * pPhoneMaterial->ks[2]);
                                            specular[3] = static_cast<UBYTE>(255.0f * pPhoneMaterial->ns);

                                            break;
                                        }
                                        default:
                                            NotImplemented();
                                    }

                                    const size_t specularDataSize = sizeof(image_data) * 4;

                                    image_data* fixValueSpecularMapData =
                                        static_cast<image_data*>(malloc(specularDataSize));

                                    assert(fixValueSpecularMapData != NULL);

                                    if (fixValueSpecularMapData != NULL)
                                    {
                                        memcpy(fixValueSpecularMapData, &(specular[0]), specularDataSize);

                                        pVulkanSpecularMap =
                                            m_pTextureMgr->createTexture2DSampler(1,
                                                                                  1,
                                                                                  1,
                                                                                  TRUE,
                                                                                  BX_FORMAT_RGBA8,
                                                                                  BX_FORMAT_RGBA8,
                                                                                  textureSamplerCreateData,
                                                                                  &(fixValueSpecularMapData[0]));
                                    }
                                }
                            }

                            // Parse normal map
                            if (pMaterialMap != NULL)
                            {
                                const ::Texture::Texture2D* pNormalMap =
                                    pMaterialMap->m_materialMapStruct.normalMap;

                                if (pNormalMap != NULL)
                                {
                                    pVulkanNormalMap =
                                        m_pTextureMgr->createTexture2DSampler(pNormalMap->GetTextureWidth(),
                                                                              pNormalMap->GetTextureHeight(),
                                                                              1,
                                                                              TRUE,
                                                                              BX_FORMAT_RGBA8,
                                                                              BX_FORMAT_RGBA8,
                                                                              textureSamplerCreateData,
                                                                              static_cast<const image_data*>(pNormalMap->GetTextureData()));
                                }
                            }

                            // Parse light map
                            if (pMaterialMap != NULL)
                            {
                                const ::Texture::Texture2D* pLightMap =
                                    pMaterialMap->m_materialMapStruct.lightMap;

                                if (pLightMap != NULL)
                                {
                                    pVulkanLightMap =
                                        m_pTextureMgr->createTexture2DSampler(pLightMap->GetTextureWidth(),
                                                                              pLightMap->GetTextureHeight(),
                                                                              1,
                                                                              TRUE,
                                                                              BX_FORMAT_RGBA8,
                                                                              BX_FORMAT_RGBA8,
                                                                              textureSamplerCreateData,
                                                                              static_cast<const image_data*>(pLightMap->GetTextureData()));
                                }
                            }

                            // Parse specular intense
                            float ns = 0.0f;
                            if (pMaterial->GetMaterialType() == MaterialType::PHONG)
                            {
                                ns = static_cast<const Object::Model::PhongMaterial*>(pMaterial)->ns;
                            }

                            m_mainSceneMeshMaterialMapResourceList.push_back(
                            {
                                meshId++,
                                pVulkanDiffuseMap,
                                pVulkanSpecularMap,
                                pVulkanNormalMap,
                                pVulkanLightMap,
                                ns
                            });
                        }
                    }
                }
            }
        }

        VulkanRenderTargetCreateData VulkanRenderBase::genAttachmentCreateData(
            const UINT                             renderSubPassIndex,
            const UINT                             bindingPoint,
            const UINT                             isStore,
            const BX_FRAMEBUFFER_ATTACHMENT_LAYOUT layout,
            const BOOL                             useStencil,
            const BOOL                             isStoreStencil)
        {
            Render::VulkanRenderTargetCreateData rtCreateData = {};
            rtCreateData.renderSubPassIndex = renderSubPassIndex;
            rtCreateData.bindingPoint       = bindingPoint;
            rtCreateData.isStore            = isStore;
            rtCreateData.layout             = layout;
            rtCreateData.useStencil         = useStencil;
            rtCreateData.isStoreStencil     = isStoreStencil;

            return rtCreateData;
        }

        void VulkanRenderBase::genBackbufferDepthBuffer()
        {
            size_t backBufferNum = m_backBufferRTsCreateDataList.size();

            for (size_t i = 0; i < backBufferNum; ++i)
            {
                Texture::VulkanTexture2D* pBackbufferColorTexture =
                    static_cast<Texture::VulkanTexture2D*>(m_backBufferRTsCreateDataList[i][0].attachment.pTex);

                TextureFormat depthBufferFormat =
                    ((IsStencilTestEnabled() == FALSE) ? BX_FORMAT_DEPTH32 : BX_FORMAT_DEPTH24_STENCIL);

                Texture::VulkanTexture2D* pBackbufferDepthTexture =
                    m_pTextureMgr->createTexture2DRenderTarget(
                        pBackbufferColorTexture->GetTextureWidth(),
                        pBackbufferColorTexture->GetTextureHeight(),
                        static_cast<UINT>(m_pSetting->m_graphicsSetting.antialasing),
                        depthBufferFormat,
                        BX_TEXTURE_USAGE_VULKAN_NONE,
                        ((m_pSetting->m_graphicsSetting.antialasing == AA_NONE) ? FALSE : TRUE));

                m_backBufferRTsCreateDataList[i].push_back(
                    { static_cast<UINT>(i), { pBackbufferDepthTexture->GetSampleNumber(), pBackbufferDepthTexture } });
            }
        }

        BOOL VulkanRenderBase::createShadowPass()
        {
            BOOL status = BX_SUCCESS;

            //  Initialize graphics pipeline properties
            VulkanRenderProperties props = {};
            props.enableBlending         = FALSE;
            props.enableColor            = FALSE;
            props.enableDepth            = TRUE;
            props.enableStencil          = FALSE;

            props.depthClearValue = { 1.0f, 0.0f };

            props.renderViewportRect =
            {
                0.0f, static_cast<float>(m_pSetting->resolution.width),
                static_cast<float>(m_pSetting->resolution.height), 0.0f
            };

            Shader::BxShaderMeta shadowmapShaderMeta          = {};
            shadowmapShaderMeta.vertexShaderInfo.shaderFile   = "ShadowMap.vert.spv";
            shadowmapShaderMeta.fragmentShaderInfo.shaderFile = "ShadowMap.frag.spv";

            /// Initialize render pass
            const UINT sampleNum = m_pSetting->m_graphicsSetting.antialasing;

            Texture::VulkanTexture2D* pShadowMapTexture =
                m_pTextureMgr->createTexture2DRenderTarget(
                    m_pSetting->resolution.width,
                    m_pSetting->resolution.height,
                    sampleNum,
                    BX_FORMAT_DEPTH32,
                    BX_TEXTURE_USAGE_VULKAN_NONE,
                    ((m_pSetting->m_graphicsSetting.antialasing == AA_NONE) ? FALSE : TRUE));

            Texture::VulkanTexture2D* pShadowMapResolveTexture = NULL;

            std::vector<VulkanRenderTargetCreateDescriptor> shadowMapDescriptors =
            {
                { TRUE, 0, 0, sampleNum, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_DEPTH_STENCIL, FALSE, FALSE }
            };

            if (m_pSetting->m_graphicsSetting.antialasing != AA_NONE)
            {
                shadowMapDescriptors.push_back(
                    { TRUE, 0, 1, sampleNum, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_DEPTH_STENCIL, FALSE, FALSE });

                pShadowMapResolveTexture =
                    m_pTextureMgr->createTexture2DRenderTarget(
                        m_pSetting->resolution.width,
                        m_pSetting->resolution.height,
                        1,
                        BX_FORMAT_DEPTH32,
                        BX_TEXTURE_USAGE_VULKAN_NONE,
                        FALSE);
            }

            VulkanRenderTargetCreateData shadowPassRTCreateData = {};
            shadowPassRTCreateData.renderSubPassIndex           = 0;
            shadowPassRTCreateData.bindingPoint                 = 0;
            shadowPassRTCreateData.attachmentIndex              = 0;
            shadowPassRTCreateData.sampleNum                    = sampleNum;
            shadowPassRTCreateData.isStore                      = TRUE;
            shadowPassRTCreateData.layout                       = BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_DEPTH_STENCIL;
            shadowPassRTCreateData.useStencil                   = FALSE;
            shadowPassRTCreateData.isStoreStencil               = FALSE;

            std::vector<VulkanRenderTargetFramebufferCreateData> shadowPassFboCreateDataList(1);
            shadowPassFboCreateDataList[0].framebufferIndex = 0;
            shadowPassFboCreateDataList[0].attachment       = { sampleNum, pShadowMapTexture };

            shadowPassRTCreateData.pRenderTargetFramebufferCreateData = &shadowPassFboCreateDataList;

            // Initialize vertex input for render pass
            VulkanRenderResources renderSources         = {};
            renderSources.vertexDescriptionBindingPoint = 0;
            renderSources.pVertexInputResourceList      = &m_mainSceneVertexInputResourceList;

            const UINT shadowPassUboSetIndex   =
                ((m_pSetting->m_graphicsSetting.renderingMethod == FORWARD_RENDERING) ||
                 (m_pSetting->m_graphicsSetting.renderingMethod == FORWARD_PLUS_RENDERING) ||
                 (m_pSetting->m_graphicsSetting.renderingMethod == FORWARD_CLUSTERED_RENDERING) == TRUE) ? 1 : 2;
            const UINT shadowPassTransUboIndex = 0;

            std::vector<VulkanUniformBufferResource> shadowPassUniformBufferResourceList =
            {
                createTransMatrixUniformBufferResource(shadowPassUboSetIndex, shadowPassTransUboIndex)
            };

            VulkanDescriptorResources shadowPassDescriptorResources  = {};
            shadowPassDescriptorResources.descriptorSetIndex         = shadowPassUboSetIndex;
            shadowPassDescriptorResources.pUniformBufferResourceList = &shadowPassUniformBufferResourceList;

            std::vector<VulkanTextureResource> resolveShadowMap(1);
            resolveShadowMap[0].setIndex     = shadowPassUboSetIndex;
            resolveShadowMap[0].bindingPoint = 0;

            resolveShadowMap[0].pTextureList.push_back(pShadowMapResolveTexture);

            if (m_pSetting->m_graphicsSetting.antialasing != Antialasing::AA_NONE)
            {
                shadowPassDescriptorResources.pResolveAttachmentList = &resolveShadowMap;
            }

            std::vector<VulkanDescriptorResources> descriptorResourceList = { shadowPassDescriptorResources };

            renderSources.pDescriptorResourceList = &descriptorResourceList;

            // Create render pass create data
            std::vector<VulkanGraphicsPipelineRenderTargetProperties> renderTargetsProps(1);
            renderTargetsProps[0].enableBlend = FALSE;

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
            subpassGraphicsPipelineCreateData.pShaderMeta                             = &shadowmapShaderMeta;
            subpassGraphicsPipelineCreateData.pResource                               = &renderSources;
            subpassGraphicsPipelineCreateData.isScenePipeline                         = TRUE;
            subpassGraphicsPipelineCreateData.pVertexInputBindingDescription          = &vertexInputBindingDescription;
            subpassGraphicsPipelineCreateData.pVertexInputAttributeDescriptionList    = &vertexAttributeDescription;

            std::vector<VulkanRenderTargetCreateData*> shadowPasspRTCreateDataList = { &shadowPassRTCreateData };

            std::vector<VulkanRenderSubpassCreateData> renderSubpassCreateDataList(1);
            renderSubpassCreateDataList[0].pSubpassGraphicsPipelineCreateData    = &subpassGraphicsPipelineCreateData;
            renderSubpassCreateDataList[0].pSubpassRenderTargetCreateDataRefList = &shadowPasspRTCreateDataList;

            std::vector<VulkanRenderTargetCreateData> shadowPassRTCreateDataList = { shadowPassRTCreateData };

            VulkanRenderpassCreateData renderPassCreateData                      = {};
            renderPassCreateData.pRenderProperties                               = &props;
            renderPassCreateData.pSubpassCreateDataList                          = &renderSubpassCreateDataList;
            renderPassCreateData.framebufferNum                                  = 1;
            renderPassCreateData.pRenderTargetCreateDataList                     = &shadowPassRTCreateDataList;

            status = AddRenderPass(BX_RENDER_PASS_PRE_RENDER, SHADOW_PASS, renderPassCreateData);

            assert(status == BX_SUCCESS);

            return status;
        }

        VulkanUniformBufferResource VulkanRenderBase::createTransMatrixUniformBufferResource(
            const UINT setIndex,
            const UINT transMatrixUboIndex)
        {
            VulkanUniformBufferResource transUniformBufferResource = {};

            const VkPhysicalDeviceProperties hwProps = Utility::VulkanUtility::GetHwProperties(*m_pHwDevice);

            // Create transform matrix uniform buffer update data
            const UINT modelNum = m_pScene->GetSceneModelNum();
            const UINT camNum = m_pScene->GetSceneCameraNum();
            const UINT objNum = camNum * modelNum;

            assert(
                setIndex <  m_descriptorSetNum &&
                objNum   <= DEFAULT_MAX_RENDER_SCENE_OBJ_NUM);

            m_transUniformbuffer.resize(objNum);

            m_descriptorUpdateDataTable[setIndex].push_back(
                { transMatrixUboIndex, m_transUniformbuffer.data() });

            Buffer::VulkanUniformBufferDynamic* pTransUniformbuffer =
                new Buffer::VulkanUniformBufferDynamic(m_pDevice,
                    hwProps.limits.minUniformBufferOffsetAlignment);

            pTransUniformbuffer->
                createUniformBuffer(*m_pHwDevice,
                                    (UINT)m_transUniformbuffer.size(),
                                    sizeof(m_transUniformbuffer[0]),
                                    static_cast<void*>(m_transUniformbuffer.data()));

            const size_t descriptorBufferIndex = m_pDescriptorBufferList.size();
            m_pDescriptorBufferList.push_back(
                std::unique_ptr<Buffer::VulkanDescriptorBuffer>(pTransUniformbuffer));

            // Build transform uniform resource
            transUniformBufferResource.shaderType       = BX_VERTEX_SHADER;
            transUniformBufferResource.setIndex         = setIndex;
            transUniformBufferResource.bindingPoint     = transMatrixUboIndex;
            transUniformBufferResource.uniformbufferNum = 1;
            transUniformBufferResource.pUniformBuffer   =
                static_cast<Buffer::VulkanUniformBufferDynamic*>(m_pDescriptorBufferList[descriptorBufferIndex].get());

            return transUniformBufferResource;
        }

        VulkanUniformBufferResource VulkanRenderBase::createMaterialUniformBufferResource(
            const UINT setIndex,
            const UINT materialUboIndex)
        {
            VulkanUniformBufferResource materialUniformBufferResource = {};

            const VkPhysicalDeviceProperties hwProps =
                Utility::VulkanUtility::GetHwProperties(*m_pHwDevice);

            // Create material uniform buffer update data
            const UINT meshNum = static_cast<UINT>(m_mainSceneMeshMaterialMapResourceList.size());
            const UINT camNum  = m_pScene->GetSceneCameraNum();
            const UINT objNum  = camNum * meshNum;

            assert(
                setIndex <  m_descriptorSetNum &&
                objNum   <= DEFAULT_MAX_RENDER_SCENE_OBJ_NUM);

            memset(m_materialUniformBuffer.data(), 0, objNum * sizeof(MaterialUbo));

            for (UINT i = 0; i < meshNum; ++i)
            {
                m_materialUniformBuffer[i].albedoMapIndex =
                    m_mainSceneMeshMaterialMapResourceList[i].meshIndex;

                m_materialUniformBuffer[i].spcularMapIndex =
                    m_mainSceneMeshMaterialMapResourceList[i].meshIndex;

                m_materialUniformBuffer[i].normalMapIndex =
                    m_mainSceneMeshMaterialMapResourceList[i].meshIndex;

                m_materialUniformBuffer[i].lightMapIndex =
                    m_mainSceneMeshMaterialMapResourceList[i].meshIndex;

                if (m_pSetting->m_graphicsSetting.shadingMethod == ShadingMethod::Phong)
                {
                    m_materialUniformBuffer[i].phongMaterial.ns =
                        m_mainSceneMeshMaterialMapResourceList[i].ns;
                }
                else if (m_pSetting->m_graphicsSetting.shadingMethod == ShadingMethod::PBR_COOK_TORRANCE)
                {
                    m_materialUniformBuffer[i].cooktorrancePbrMaterial.roughness = 0.3f;
                    m_materialUniformBuffer[i].cooktorrancePbrMaterial.fresnel   = Math::Vector3(0.5f, 0.5f, 0.5f);
                    m_materialUniformBuffer[i].cooktorrancePbrMaterial.metallic  = 0.5f;
                }
                else
                {
                    NotImplemented();
                }
            }

            m_descriptorUpdateDataTable[setIndex].push_back(
                { materialUboIndex, m_materialUniformBuffer.data() });

            Buffer::VulkanUniformBufferDynamic* pMaterialUniformbuffer =
                new Buffer::VulkanUniformBufferDynamic(m_pDevice,
                                                        hwProps.limits.minUniformBufferOffsetAlignment);

            pMaterialUniformbuffer->
                createUniformBuffer(*m_pHwDevice,
                                    (UINT)m_materialUniformBuffer.size(),
                                    sizeof(m_materialUniformBuffer[0]),
                                    static_cast<void*>(m_materialUniformBuffer.data()));

            const size_t descriptorBufferIndex = m_pDescriptorBufferList.size();
            m_pDescriptorBufferList.push_back(
                std::unique_ptr<Buffer::VulkanDescriptorBuffer>(pMaterialUniformbuffer));

            // Build material uniform resource
            materialUniformBufferResource.shaderType       = BX_FRAGMENT_SHADER;
            materialUniformBufferResource.setIndex         = setIndex;
            materialUniformBufferResource.bindingPoint     = materialUboIndex;
            materialUniformBufferResource.uniformbufferNum = 1;
            materialUniformBufferResource.pUniformBuffer   =
                static_cast<Buffer::VulkanUniformBufferDynamic*>(m_pDescriptorBufferList[descriptorBufferIndex].get());

            return materialUniformBufferResource;
        }

        VulkanUniformBufferResource VulkanRenderBase::createLightUniformBufferResource(
            const UINT setIndex,
            const UINT lightUboIndex)
        {
            assert(setIndex < m_descriptorSetNum);

            VulkanUniformBufferResource lightUniformBufferResource = {};

            // Create light uniform buffer update data
            memset(&m_lightUbo, 0, sizeof(DynamicLightUbo));

            const UINT lightNum = m_pScene->GetSceneLightNum();
            for (UINT lightIndex = 0; lightIndex < lightNum; ++lightIndex)
            {
                const Object::Light::LightBase* pLight = m_pScene->GetLight(lightIndex);

                switch (pLight->GetLightType())
                {
                    case DIRECTIONAL_LIGHT:
                    {
                        const Object::Light::DirectionalLight* pDirectionalLight =
                            static_cast<const Object::Light::DirectionalLight*>(pLight);

                        DirectionalLightUbo* pDirectionalLightUbo =
                            &(m_lightUbo.directionalLightList[m_lightUbo.directionalLightNum]);

                        if (pDirectionalLight->IsEnable() == TRUE)
                        {
                            pDirectionalLightUbo->color = pDirectionalLight->GetLightColorVec4();
                        }

                        pDirectionalLightUbo->direction = Math::Vector4(pDirectionalLight->GetDir(), 0.0f);

                        m_lightUbo.directionalLightNum++;

                        break;
                    }
                    case POINT_LIGHT:
                    {
                        const Object::Light::PointLight* pPointLight =
                            static_cast<const Object::Light::PointLight*>(pLight);

                        PointLightUbo* pPointLightUbo =
                            &(m_lightUbo.pointLightList[m_lightUbo.pointLightNum]);

                        if (pPointLight->IsEnable() == TRUE)
                        {
                            pPointLightUbo->color = pPointLight->GetLightColorVec4();
                        }

                        pPointLightUbo->position = pPointLight->GetPos();
                        pPointLightUbo->range = pPointLight->GetRadius();

                        m_lightUbo.pointLightNum++;

                        break;
                    }
                    case SPOT_LIGHT:
                    {
                        const Object::Light::SpotLight* pSpotLight =
                            static_cast<const Object::Light::SpotLight*>(pLight);

                        SpotLightUbo* pSpotLightUbo =
                            &(m_lightUbo.spotLightList[m_lightUbo.spotLightNum]);

                        if (pSpotLight->IsEnable() == TRUE)
                        {
                            pSpotLightUbo->color = pSpotLight->GetLightColorVec4();
                        }

                        pSpotLightUbo->position          = pSpotLight->GetPos();
                        pSpotLightUbo->direction         = pSpotLight->GetDir();
                        pSpotLightUbo->innerRadiusCosine = pSpotLight->GetInnerAngleCosine();
                        pSpotLightUbo->outerRadiusCosine = pSpotLight->GetOuterAngleCosine();

                        m_lightUbo.spotLightNum++;

                        break;
                    }
                    default:
                    {
                        NotSupported();
                        break;
                    }
                }
            }

            const size_t descriptorBufferIndex = m_pDescriptorBufferList.size();
            m_descriptorUpdateDataTable[setIndex].push_back({ lightUboIndex, &m_lightUbo });

            // Create light uniform buffer
            Buffer::VulkanUniformBuffer* pLightUniformBuffer = new Buffer::VulkanUniformBuffer(m_pDevice);
            pLightUniformBuffer->createUniformBuffer(*m_pHwDevice, 1, sizeof(m_lightUbo), static_cast<void*>(&m_lightUbo));

            m_pDescriptorBufferList.push_back(std::unique_ptr<Buffer::VulkanDescriptorBuffer>(pLightUniformBuffer));

            // Build light uniform resource
            lightUniformBufferResource.shaderType       = BX_FRAGMENT_SHADER;
            lightUniformBufferResource.setIndex         = setIndex;
            lightUniformBufferResource.bindingPoint     = lightUboIndex;
            lightUniformBufferResource.uniformbufferNum = 1;
            lightUniformBufferResource.pUniformBuffer   =
                static_cast<Buffer::VulkanUniformBuffer*>(m_pDescriptorBufferList[descriptorBufferIndex].get());

            return lightUniformBufferResource;
        }

        VulkanUniformBufferResource VulkanRenderBase::createCamUniformBufferResource(
            const Object::Camera::CameraBase* const pCam,
            const UINT                              setIndex,
            const UINT                              camUboIndex)
        {
            assert(setIndex < m_descriptorSetNum);

            VulkanUniformBufferResource camUniformBufferResource = {};

            /// Camera position uniform buffer
            // Create camera position uniform buffer update data
            m_descriptorUpdateDataTable[setIndex].push_back(
                { camUboIndex,
                  const_cast<Math::Vector3*>(&(m_pScene->GetCamera(0)->GetTrans()->GetPos())) });

            Buffer::VulkanUniformBuffer* pCameraPositionUniformBuffer = new Buffer::VulkanUniformBuffer(m_pDevice);
            pCameraPositionUniformBuffer->createUniformBuffer(
                *m_pHwDevice, 1, sizeof(Math::Vector3), static_cast<const void*>(&(pCam->GetTrans()->GetPos())));

            const size_t descriptorBufferIndex = m_pDescriptorBufferList.size();

            m_pDescriptorBufferList.push_back(
                std::unique_ptr<Buffer::VulkanDescriptorBuffer>(pCameraPositionUniformBuffer));

            // Build camera uniform resource
            camUniformBufferResource.shaderType       = BX_FRAGMENT_SHADER;
            camUniformBufferResource.setIndex         = setIndex;
            camUniformBufferResource.bindingPoint     = camUboIndex;
            camUniformBufferResource.uniformbufferNum = 1;
            camUniformBufferResource.pUniformBuffer   =
                static_cast<Buffer::VulkanUniformBuffer*>(m_pDescriptorBufferList[descriptorBufferIndex].get());

            return camUniformBufferResource;
        }

        VulkanTextureResource VulkanRenderBase::createSceneTextures(
            const UINT                                    setIndex,
            const UINT                                    bindingPoint,
            const std::vector<Texture::VulkanTexture2D*>& texturePtrList)
        {
            VulkanTextureResource textureResource = {};

            textureResource.setIndex     = setIndex;
            textureResource.bindingPoint = bindingPoint;
            textureResource.shaderType   = BX_FRAGMENT_SHADER;

            const size_t textureNum = texturePtrList.size();
            textureResource.pTextureList.resize(textureNum);

            for (size_t texIndex = 0; texIndex < textureNum; ++texIndex)
            {
                textureResource.pTextureList[texIndex] =
                    static_cast<Texture::VulkanTextureBase*>(texturePtrList[texIndex]);
            }

            return textureResource;
        }
    }
}