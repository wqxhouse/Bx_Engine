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
                const Scene::RenderScene*                           pScene,
                const std::vector<Texture::VulkanTexture2D*>* const ppBackbufferTextures);

            ~VulkanRenderBase();

            virtual BOOL initialize() = 0;

            virtual void update(const float delta) = 0;
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

        protected:
            void parseScene();

            const Setting*          const m_pSetting;
            const VkPhysicalDevice* const m_pHwDevice;
            const VkDevice*         const m_pDevice;
            Mgr::CmdBufferMgr*      const m_pCmdBufferMgr;
            Mgr::DescriptorMgr*     const m_pDescritorMgr;
            const Scene::RenderScene*     m_pScene;

            std::vector<VulkanRenderPass> m_preDrawPassList;
            VulkanRenderPass              m_mainSceneRenderPass;
            std::vector<VulkanRenderPass> m_postDrawPassList;

            const std::vector<Texture::VulkanTexture2D*>* const m_ppBackbufferTextures;
            std::vector<VulkanVertexInputResources>             m_mainSceneVertexInputResourceList;

        private:
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
                const Scene::RenderScene*                           pScene,
                const std::vector<Texture::VulkanTexture2D*>* const ppBackbufferTextures);

            ~VulkanForwardRender();

            BOOL initialize();

            void update(
                const float delta);

            void draw();

        private:
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
                const Scene::RenderScene*                           pScene,
                const std::vector<Texture::VulkanTexture2D*>* const ppBackbufferTextures);

            ~VulkanDeferredRender();

            void update(const float delta);
            void draw();

        private:
        };
    }
}