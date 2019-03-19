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
                const VulkanRenderpassCreateData& renderPassCreateData)
            {
                switch (renderPassStage)
                {
                    case BX_RENDER_PASS_PRE_RENDER:
                    {
                        size_t prePassSize = m_preDrawPassList.size();

                        m_preDrawPassList.push_back(VulkanRenderPass(m_pSetting,
                                                                     m_pDevice,
                                                                     m_pCmdBufferMgr,
                                                                     m_pDescritorMgr,
                                                                     m_pScene));

                        m_preDrawPassList.at(prePassSize).create(renderPassCreateData);

                        break;
                    }
                    case BX_RENDER_PASS_MAIN_SCENE_RENDER:
                    {
                        m_mainSceneRenderPass.clean();

                        m_mainSceneRenderPass.create(renderPassCreateData);
                        break;
                    }
                    case BX_RENDER_PASS_POST_RENDER:
                    {
                        size_t postPassSize = m_postDrawPassList.size();

                        m_postDrawPassList.push_back(VulkanRenderPass(m_pSetting,
                                                                      m_pDevice,
                                                                      m_pCmdBufferMgr,
                                                                      m_pDescritorMgr,
                                                                      m_pScene));

                        m_postDrawPassList.at(postPassSize).create(renderPassCreateData);

                        break;
                    }
                    default:
                        NotSupported();
                        break;
                }
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

            VulkanUniformBufferResource createTransMatrixUniformBufferResource(
                const UINT setIndex,
                const UINT transMatrixUboIndex);

            VulkanUniformBufferResource createLightUniformBufferResource(
                const UINT setIndex,
                const UINT lightUboIndex);

            VulkanUniformBufferResource createCamUniformBufferResource(
                const UINT setIndex,
                const UINT camUboIndex);

            VulkanTextureResource createSceneTextures(
                const UINT                setIndex,
                const UINT                bindingPoint,
                const UINT                textureNum,
                Texture::VulkanTexture2D* pTexture);

            const Setting*          const m_pSetting;
            const VkPhysicalDevice* const m_pHwDevice;
            const VkDevice*         const m_pDevice;
            Mgr::CmdBufferMgr*      const m_pCmdBufferMgr;
            Mgr::DescriptorMgr*     const m_pDescritorMgr;
            Mgr::TextureMgr*        const m_pTextureMgr;
            const Scene::RenderScene*     m_pScene;

            std::vector<VulkanRenderPass> m_preDrawPassList;
            VulkanRenderPass              m_mainSceneRenderPass;
            std::vector<VulkanRenderPass> m_postDrawPassList;

            const std::vector<Texture::VulkanTexture2D*>* const m_ppBackbufferTextures;

            std::vector<VulkanVertexInputResource>              m_mainSceneVertexInputResourceList;
            std::vector<VulkanMeshMaterialMapResource>          m_mainSceneMeshMaterialMapResourceList;

            UINT                                                         m_descriptorSetNum;
            std::vector<std::unique_ptr<Buffer::VulkanDescriptorBuffer>> m_pDescriptorBufferList;
            std::vector<std::vector<VulkanDescriptorUpdateData>>         m_descriptorUpdateDataTable;

            std::vector<std::vector<VulkanRenderTargetFramebufferCreateData>> m_backBufferRTsCreateDataList;

            std::vector<TransUbo>                   m_transUniformbuffer;

            DynamicLightUbo                         m_lightUbo;

        private:
            BOOL m_isDepthTestEnabled;
            BOOL m_isStencilTestEnabled;
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