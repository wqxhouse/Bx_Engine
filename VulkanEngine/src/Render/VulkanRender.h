//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2019
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

            ~VulkanRenderBase();

            virtual BOOL initialize() = 0;

            virtual BOOL update(const float delta) = 0;
            virtual void draw() = 0;

            INLINE BOOL AddRenderPass(
                const BX_RENDER_PASS_STAGE        renderPassStage,
                const VulkanRenderPassCreateData& renderPassCreateData)
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

            struct DynamicLightUbo
            {
                Object::Light::DirectionalLight m_directionalLightList[MAX_DIRECTIONAL_LIGHT_NUM];
                Object::Light::PointLight       m_pointLightList[MAX_DYNAMIC_POINT_LIGHT_NUM];
                Object::Light::SpotLight        m_spotLightList[MAX_DYNAMIC_SPOT_LIGHT_NUM];
            };

            void parseScene();
            VulkanRenderTargetCreateData genAttachmentCreateData(
                const UINT                             renderSubPassIndex,
                const UINT                             bindingPoint,
                const UINT                             isStore,
                const BX_FRAMEBUFFER_ATTACHMENT_LAYOUT layout,
                const BOOL                             useStencil,
                const BOOL                             isStoreStencil);

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

            std::vector<VulkanVertexInputResource> m_mainSceneVertexInputResourceList;

            std::vector<std::unique_ptr<Buffer::VulkanDescriptorBuffer>> m_pDescriptorBufferList;

            // const std::vector<Texture::VulkanTexture2D*>* const m_ppBackbufferTextures;
            std::vector<std::vector<VulkanRenderTargetFramebufferCreateData>> m_backBufferRTsCreateDataList;

            std::vector<VulkanDescriptorUpdateData> m_descriptorUpdateDataList;
            std::vector<TransUbo>                   m_transUniformbuffer;

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
            struct ForwardRenderMainSceneUbo
            {
                Math::Mat4 transMat;
            };
            
            std::vector<ForwardRenderMainSceneUbo> m_forwardRenderMainSceneUbo;
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

            BOOL update(const float delta);
            void draw();

        private:
        };
    }
}