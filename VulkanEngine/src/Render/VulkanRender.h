//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "VulkanRenderPass.h"
#include "../Context/TextureMgr.h"

#define BACKBUFFER_PRESENT_ATTACHMENT_INDEX 0

#define TRANS_UBO_WORLD_MATRIX_INDEX 0
#define TRANS_UBO_VIEW_MATRIX_INDEX  1
#define TRANS_UBO_PROJ_MATRIX_INDEX  2

namespace VulkanEngine
{
    namespace Render
    {
        class VulkanRenderBase
        {
        public:
            VulkanRenderBase(
                const Setting*                                const pSetting,
                const VkPhysicalDevice*                       const pHwDevice,
                const VkDevice*                               const pDevice,
                Mgr::CmdBufferMgr*                            const pCmdBufferMgr,
                Mgr::DescriptorMgr*                           const pDescritorMgr,
                Mgr::TextureMgr*                              const pTextureMgr,
                const Scene::RenderScene*                           pScene,
                const std::vector<Texture::VulkanTexture2D*>* const ppBackbufferTextures);

            virtual ~VulkanRenderBase();

            virtual BOOL initialize() = 0;

            virtual BOOL update(const float delta) = 0;
            virtual void draw() = 0;

            INLINE BOOL AddRenderPass(
                const BX_RENDER_PASS_STAGE        renderPassStage,
                const RenderPassType              renderPassType,
                const VulkanRenderpassCreateData& renderPassCreateData)
            {
                BOOL status = BX_SUCCESS;

                switch (renderPassStage)
                {
                    case BX_RENDER_PASS_PRE_RENDER:
                    {
                        m_preDrawPassList.push_back(VulkanRenderPass(m_pSetting,
                                                                     m_pDevice,
                                                                     m_pCmdBufferMgr,
                                                                     m_pDescriptorMgr,
                                                                     m_pScene,
                                                                     m_renderPassNum,
                                                                     renderPassType));

                        m_renderPassNum++;

                        status = m_preDrawPassList.back().create(renderPassCreateData);

                        break;
                    }
                    case BX_RENDER_PASS_MAIN_SCENE_RENDER:
                    {
                        m_mainSceneRenderPass.clean();

                        status = m_mainSceneRenderPass.create(renderPassCreateData);
                        break;
                    }
                    case BX_RENDER_PASS_POST_RENDER:
                    {
                        m_postDrawPassList.push_back(VulkanRenderPass(m_pSetting,
                                                                      m_pDevice,
                                                                      m_pCmdBufferMgr,
                                                                      m_pDescriptorMgr,
                                                                      m_pScene,
                                                                      m_renderPassNum,
                                                                      renderPassType));

                        m_renderPassNum++;

                        status = m_postDrawPassList.back().create(renderPassCreateData);

                        break;
                    }
                    default:
                        NotSupported();
                        break;
                }

                return status;
            }

            INLINE const std::vector<VulkanRenderPass>& GetPreDrawPassList() const
            {
                return m_preDrawPassList;
            }

            INLINE const std::vector<VulkanRenderPass>& GetPostDrawPassList() const
            {
                return m_postDrawPassList;
            }

            INLINE BOOL IsDepthTestEnabled()   const { return m_isDepthTestEnabled;     }
            INLINE BOOL IsStencilTestEnabled() const { return m_isStencilTestEnabled;   }

            INLINE void EnableDepthTest()            { m_isDepthTestEnabled = TRUE;     }
            INLINE void DisableDepthTest()           { m_isDepthTestEnabled = FALSE;    }
            INLINE void EnableStencilTest()          { m_isStencilTestEnabled = TRUE;   }
            INLINE void DisableStencilTest()         { m_isStencilTestEnabled = FALSE;  }

        protected:
            struct VulkanRenderTargetCreateDescriptor
            {
                BOOL                             isStore;
                UINT                             renderSubPassIndex;
                UINT                             bindingPoint;
                UINT                             sampleNum;
                BX_FRAMEBUFFER_ATTACHMENT_LAYOUT layout;
                BOOL                             useStencil;
                BOOL                             isStoreStencil;
            };

            struct TransUbo
            {
                Math::Mat4 worldMat;
                Math::Mat4 viewMat;
                Math::Mat4 projMat;
                Math::Mat4 wvpMat;
            };

            struct MaterialUbo
            {
                MaterialUbo() {}
                ~MaterialUbo() {}

                UINT albedoMapIndex;
                UINT spcularMapIndex;
                UINT normalMapIndex;
                UINT lightMapIndex;

                union
                {
                    struct PhongMaterial
                    {
                        float ns;
                        // TODO: Avoid align the dynamic uniform buffer with hard code
                        // use the minUniformBufferOffsetAlignment from hw property to
                        // creat the UBO for dynamic uniform buffer
                        Math::Vector3 paddingNS;
                        Math::Vector4 reserve[14];
                    } phongMaterial;

                    struct CookTorrancePbrMaterial
                    {
                        float roughness;       Math::Vector3 padding0;
                        Math::Vector3 fresnel; float         padding1;
                        float metallic;        Math::Vector3 padding2;
                        Math::Vector4 reserve[12];
                    } cooktorrancePbrMaterial;
                };
            };

            struct LightBaseUbo
            {
                Math::Vector4 color;
                Math::Mat4    lightTransVP;
            };

            struct DirectionalLightUbo : public LightBaseUbo
            {
                Math::Vector4 direction;
            };

            struct PointLightUbo : public LightBaseUbo
            {
                Math::Vector3 position;
                float         range;
            };

            struct SpotLightUbo : public LightBaseUbo
            {
                Math::Vector3 position;
                float         innerRadiusCosine;

                Math::Vector3 direction;
                float         outerRadiusCosine;
            };

            struct DynamicLightUbo
            {
                DynamicLightUbo() : directionalLightNum(0),
                                    pointLightNum(0),
                                    spotLightNum(0) {}

                UINT                directionalLightNum; Math::Vector3 padding;
                DirectionalLightUbo directionalLightList[MAX_DIRECTIONAL_LIGHT_NUM];

                UINT                pointLightNum; Math::Vector3 padding2;
                PointLightUbo       pointLightList[MAX_DYNAMIC_POINT_LIGHT_NUM];

                UINT                spotLightNum; Math::Vector3 padding3;
                SpotLightUbo        spotLightList[MAX_DYNAMIC_SPOT_LIGHT_NUM];
            };

            void parseScene();

            VulkanRenderTargetCreateData genAttachmentCreateData(
                const UINT                             renderSubPassIndex,
                const UINT                             bindingPoint,
                const UINT                             isStore,
                const BX_FRAMEBUFFER_ATTACHMENT_LAYOUT layout,
                const BOOL                             useStencil,
                const BOOL                             isStoreStencil);

            void genBackbufferDepthBuffer();

            BOOL createShadowPass();

            VulkanUniformBufferResource createTransMatrixUniformBufferResource(
                const UINT setIndex,
                const UINT transMatrixUboIndex);

            VulkanUniformBufferResource createMaterialUniformBufferResource(
                const UINT setIndex,
                const UINT materialUboIndex);

            VulkanUniformBufferResource createLightUniformBufferResource(
                const UINT setIndex,
                const UINT lightUboIndex);

            VulkanUniformBufferResource createCamUniformBufferResource(
                const Object::Camera::CameraBase* const pCam,
                const UINT                              setIndex,
                const UINT                              camUboIndex);

            VulkanTextureResource createSceneTextures(
                const UINT                                    setIndex,
                const UINT                                    bindingPoint,
                const std::vector<Texture::VulkanTexture2D*>& texturePtrList);

            const Setting*          const                                     m_pSetting;
            const VkPhysicalDevice* const                                     m_pHwDevice;
            const VkDevice*         const                                     m_pDevice;
            Mgr::CmdBufferMgr*      const                                     m_pCmdBufferMgr;
            Mgr::DescriptorMgr*     const                                     m_pDescriptorMgr;
            Mgr::TextureMgr*        const                                     m_pTextureMgr;
            const Scene::RenderScene*                                         m_pScene;

            std::vector<VulkanRenderPass>                                     m_preDrawPassList;
            VulkanRenderPass                                                  m_mainSceneRenderPass;
            std::vector<VulkanRenderPass>                                     m_postDrawPassList;
            UINT                                                              m_renderPassNum;

            const std::vector<Texture::VulkanTexture2D*>* const               m_ppBackbufferTextures;

            std::vector<VulkanVertexInputResource>                            m_mainSceneVertexInputResourceList;
            std::vector<VulkanMeshMaterialMapResource>                        m_mainSceneMeshMaterialMapResourceList;

            UINT                                                              m_descriptorSetNum;
            std::vector<std::unique_ptr<Buffer::VulkanDescriptorBuffer>>      m_pDescriptorBufferList;
            std::vector<std::vector<VulkanDescriptorUpdateData>>              m_descriptorUpdateDataTable;

            std::vector<std::vector<VulkanRenderTargetFramebufferCreateData>> m_backBufferRTsCreateDataList;

            std::vector<TransUbo>                                             m_transUniformbuffer;

            std::vector<MaterialUbo>                                          m_materialUniformBuffer;

            DynamicLightUbo                                                   m_lightUbo;

        private:
            BOOL                                                              m_isDepthTestEnabled;
            BOOL                                                              m_isStencilTestEnabled;
        };

        class VulkanForwardRender : public VulkanRenderBase
        {
        public:
            VulkanForwardRender(
                const Setting*                                const pSetting,
                const VkPhysicalDevice*                       const pHwDevice,
                const VkDevice*                               const pDevice,
                Mgr::CmdBufferMgr*                            const pCmdBufferMgr,
                Mgr::DescriptorMgr*                           const pDescritorMgr,
                Mgr::TextureMgr*                              const pTextureMgr,
                const Scene::RenderScene*                           pScene,
                const std::vector<Texture::VulkanTexture2D*>* const ppBackbufferTextures);

            ~VulkanForwardRender();

            BOOL initialize();

            BOOL update(const float deltaTime);

            void draw();

        private:
            BOOL createMainSceneRenderPass();

            void initializeBackbufferRTCreateData();

            std::vector<VulkanUniformBufferResource> createUniformBufferResource();
        };

        class VulkanDeferredRender : public VulkanRenderBase
        {
        public:
            VulkanDeferredRender(
                const Setting*                                const pSetting,
                const VkPhysicalDevice*                       const pHwDevice,
                const VkDevice*                               const pDevice,
                Mgr::CmdBufferMgr*                            const pCmdBufferMgr,
                Mgr::DescriptorMgr*                           const pDescritorMgr,
                Mgr::TextureMgr*                              const pTextureMgr,
                const Scene::RenderScene*                           pScene,
                const std::vector<Texture::VulkanTexture2D*>* const ppBackbufferTextures);

            ~VulkanDeferredRender();

            BOOL initialize();

            BOOL update(const float delta);
            void draw();

        private:
            struct MsaaUniformBuffer
            {
                INT        sampleNum; const INT padding = 0;
                Resolution dimension;
            } m_msaaUniformBuffer;

            void initializeBackbufferRTCreateData();

            void createGBufferTextures();

            VulkanUniformBufferResource createViewMatrixUniformBufferResource(
                const UINT setIndex,
                const UINT viewMatrixUboIndex);

            VulkanUniformBufferResource createMsaaUniformBufferResource(
                const UINT setIndex,
                const UINT msaaUboIndex);

            std::vector<VulkanUniformBufferResource> createGbufferUniformBufferResource();
            std::vector<VulkanUniformBufferResource> createShadingPassUniformBufferResource();

            std::vector<VulkanRenderTargetCreateData> genRenderTargetsCreateData(
                IN  const std::vector<VulkanRenderTargetCreateDescriptor>&             RTDescList,
                OUT std::vector<std::vector<VulkanRenderTargetFramebufferCreateData>>* pRTFrameBuffersCreateDataList);

            VulkanRenderSubpassCreateData genGBufferSubpassCreateData(
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
                OUT VulkanSubpassGraphicsPipelineCreateData*                   pGBufferGraphicsPipelineCreateData);

            VulkanRenderSubpassCreateData genShadingPassCreateData(
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
                OUT VulkanSubpassGraphicsPipelineCreateData*                   pShadingPassGraphicsPipelineCreateData);

            std::vector<VulkanVertexInputResource> m_shadingPassVertexInput;

            float m_shadingPassQuad[8] =
            {
                -1.0f, -1.0f,
                -1.0f,  1.0f,
                 1.0f, -1.0f,
                 1.0f,  1.0f
            };

            std::vector<UINT> m_shadingPassQuadIndices =
            {
                0, 1, 2,
                2, 1, 3
            };
        };
    }
}