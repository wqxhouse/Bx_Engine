//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include <Scene/RenderScene.h>
#include <Math/Structures.h>

#include "VulkanGraphicsPipeline.h"

namespace VulkanEngine
{
    namespace Render
    {
        struct VulkanRenderTargetFramebufferCreateData
        {
            UINT                        framebufferIndex;
            Texture::VulkanTextureBase* pTexture;
        };

        struct VulkanRenderSubpassCreateData
        {
            BOOL                                                  isStore;
            UINT                                                  renderSubpassIndex;
            UINT                                                  bindingPoint;
            BX_FRAMEBUFFER_ATTACHMENT_LAYOUT                      layout;
            BOOL                                                  useStencil;
            BOOL                                                  isStoreStencil;
            UINT                                                  renderTargetNum;
            UINT                                                  renderFramebufferNum;
            std::vector<VulkanRenderTargetFramebufferCreateData>* pRenderTargetFramebufferCreateData;
        };

        struct VulkanRenderPassCreateData
        {
            // Graphics Pipelines create data
            std::vector<VulkanGraphicsPipelineCreateData>* pGraphicsPipelineCreateDataList;

            // Render Target create data
            std::vector<VulkanRenderSubpassCreateData>*    pRenderSubpassCreateDataList;
        };

        struct VulkanDescriptorUpdateData
        {
            UINT  descriptorIndex;
            void* pData;
            BOOL  isDataUpdated;
        };

        class VulkanRenderPass
        {
        public:
            VulkanRenderPass(
                const Setting*      const pSetting,
                const VkDevice*     const pDevice,
                Mgr::CmdBufferMgr*  const pCmdBufferMgr,
                Mgr::DescriptorMgr* const pDescritorMgr,
                const Scene::RenderScene* pScene);

            ~VulkanRenderPass();

            BOOL create(
                const VulkanRenderPassCreateData& renderPassCreateData);

            BOOL update(
                const float                                    deltaTime,
                const std::vector<VulkanDescriptorUpdateData>& updateDataList);

            BOOL draw();

            void clean();

            INLINE const Buffer::VulkanFramebuffer* GetFramebuffer(
                const UINT subpassIndex,
                const UINT framebufferIndex) const
            {
                return &(m_framebufferList[subpassIndex][framebufferIndex]);
            }

        private:
            /*struct VulkanRenderPassUniformBufferUpdateData
            {
                Mgr::DescriptorUpdateInfo* pUpdateInfo;
                void*                      data;
                BOOL                       isUpdate;
            };*/

            // Create VkRenderPass and generate framebuffers
            BOOL createRenderPass(
                const std::vector<VulkanRenderSubpassCreateData>* pRenderSubpassCreateDataList);

            // Context
            const Setting*            m_pSetting;
            const VkDevice*           m_pDevice;
            Mgr::CmdBufferMgr* const  m_pCmdBufferMgr;
            Mgr::DescriptorMgr* const m_pDescriptorMgr;

            VDeleter<VkRenderPass>    m_renderPass;

            VulkanGraphicsPipeline    m_graphicsPipeline;

            // Resources
            const Scene::RenderScene* m_pScene;

            std::vector<std::vector<Buffer::VulkanFramebuffer>> m_framebufferList;

            std::vector<UINT> m_renderTargetNumList;
        };
    }
}