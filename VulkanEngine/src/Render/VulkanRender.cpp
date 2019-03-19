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
              m_pDescritorMgr(pDescritorMgr),
              m_pTextureMgr(pTextureMgr),
              m_pScene(pScene),
              m_mainSceneRenderPass(pSetting, pDevice, pCmdBufferMgr, pDescritorMgr, pScene),
              m_isDepthTestEnabled(FALSE),
              m_isStencilTestEnabled(FALSE),
              m_ppBackbufferTextures(ppBackbufferTextures)
        {
            assert(ppBackbufferTextures->size() > 0);

            const size_t backBufferNum = ppBackbufferTextures->size();
            m_backBufferRTsCreateDataList.resize(backBufferNum);

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

                        const Object::Model::MaterialMap* pMaterialMap = pMesh->GetMaterialMap();

                        Texture::VulkanTexture2D* pVulkanDiffuseMap  = NULL;
                        Texture::VulkanTexture2D* pVulkanSpecularMap = NULL;
                        Texture::VulkanTexture2D* pVulkanNormalMap   = NULL;
                        Texture::VulkanTexture2D* pVulkanLightMap    = NULL;

                        // Parse diffuse map
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
                        }

                        // Parse specular map
                        {
                            const ::Texture::Texture2D* pSpecularMap =
                                pMaterialMap->m_materialMapStruct.specMap;

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
                        }

                        // Parse normal map
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

                        m_mainSceneMeshMaterialMapResourceList.push_back(
                        {
                            meshId++,
                            std::unique_ptr<Texture::VulkanTexture2D>(pVulkanDiffuseMap),
                            std::unique_ptr<Texture::VulkanTexture2D>(pVulkanSpecularMap),
                            std::unique_ptr<Texture::VulkanTexture2D>(pVulkanNormalMap),
                            std::unique_ptr<Texture::VulkanTexture2D>(pVulkanLightMap)
                        });
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

                Texture::VulkanTexture2D* backbufferDepthTexture =
                    m_pTextureMgr->createTexture2DRenderTarget(
                        pBackbufferColorTexture->GetTextureWidth(),
                        pBackbufferColorTexture->GetTextureHeight(),
                        static_cast<UINT>(m_pSetting->m_graphicsSetting.antialasing),
                        depthBufferFormat,
                        BX_TEXTURE_USAGE_VULKAN_NONE,
                        ((m_pSetting->m_graphicsSetting.antialasing == AA_NONE) ? FALSE : TRUE));

                m_backBufferRTsCreateDataList[i].push_back(
                    { static_cast<UINT>(i), { backbufferDepthTexture->GetSampleNumber(), backbufferDepthTexture} });
            }
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

            Buffer::VulkanUniformBufferDynamic * pMainSceneUniformbuffer =
                new Buffer::VulkanUniformBufferDynamic(m_pDevice,
                    hwProps.limits.minUniformBufferOffsetAlignment);

            pMainSceneUniformbuffer->createUniformBuffer(*m_pHwDevice,
                (UINT)m_transUniformbuffer.size(),
                sizeof(m_transUniformbuffer[0]),
                static_cast<void*>(m_transUniformbuffer.data()));

            const size_t descriptorBufferIndex = m_pDescriptorBufferList.size();
            m_pDescriptorBufferList.push_back(
                std::unique_ptr<Buffer::VulkanDescriptorBuffer>(pMainSceneUniformbuffer));

            // Build transform uniform resource
            transUniformBufferResource.shaderType       = BX_VERTEX_SHADER;
            transUniformBufferResource.setIndex         = setIndex;
            transUniformBufferResource.bindingPoint     = transMatrixUboIndex;
            transUniformBufferResource.uniformbufferNum = 1;
            transUniformBufferResource.pUniformBuffer   =
                static_cast<Buffer::VulkanUniformBufferDynamic*>(m_pDescriptorBufferList[descriptorBufferIndex].get());

            return transUniformBufferResource;
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
            const UINT setIndex,
            const UINT camUboIndex)
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
                *m_pHwDevice, 1, sizeof(Math::Vector3), static_cast<const void*>(&(m_pScene->GetCamera(0)->GetTrans()->GetPos())));

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
            const UINT                setIndex,
            const UINT                bindingPoint,
            const UINT                textureNum,
            Texture::VulkanTexture2D* pTexture)
        {
            VulkanTextureResource textureResource = {};

            textureResource.setIndex     = setIndex;
            textureResource.bindingPoint = bindingPoint;
            textureResource.textureNum   = textureNum;
            textureResource.pTexture     = pTexture;
            textureResource.shaderType   = BX_FRAGMENT_SHADER;

            return textureResource;
        }
    }
}