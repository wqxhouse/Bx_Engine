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

        struct VulkanRenderTargetCreateData
        {
            BOOL                                                  isStore;
            UINT                                                  renderSubPassIndex;
            UINT                                                  bindingPoint;
            BX_FRAMEBUFFER_ATTACHMENT_LAYOUT                      layout;
            BOOL                                                  useStencil;
            BOOL                                                  isStoreStencil;
            std::vector<VulkanRenderTargetFramebufferCreateData>* pRenderTargetFramebufferCreateData;
        };

        struct VulkanRenderSubpassCreateData
        {
            // GraphicsPipeline create data
            UINT                                           framebufferNum;
            VulkanGraphicsPipelineCreateData*              pGraphicsPipelineCreateData;
            std::vector<VulkanRenderTargetCreateData>*     pRenderTargetCreateDataList;
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
                const std::vector<VulkanRenderSubpassCreateData>& renderPassCreateData);

            BOOL update(
                const float                                    deltaTime,
                const std::vector<VulkanDescriptorUpdateData>& updateDataList);

            BOOL draw();

            void clean();

            INLINE const Buffer::VulkanFramebuffer* GetFramebuffer(
                const UINT framebufferIndex) const
            {
                return &(m_framebufferList[framebufferIndex]);
            }

        private:
            /*struct VulkanRenderPassUniformBufferUpdateData
            {
                Mgr::DescriptorUpdateInfo* pUpdateInfo;
                void*                      data;
                BOOL                       isUpdate;
            };*/

            // Create render targets for a single subpass
            BOOL createRenderTargets(
                IN  const std::vector<VulkanRenderTargetCreateData>*       pRenderTargetsCreateDataList,
                IN  const BOOL                                             enableDepth,
                OUT VkSubpassDescription*                                  pSubpassDescription,
                OUT std::vector<VkAttachmentDescription>*                  pAttachmentDescriptionList,
                OUT std::vector<VkAttachmentReference>*                    pColorSubpassAttachmentRefList,
                OUT VkAttachmentReference*                                 pDepthSubpassAttachmentRef,
                OUT std::vector<std::vector<Texture::VulkanTextureBase*>>* pFramebuffersTextureTable);

            // Create render pass
            BOOL createRenderPass(
                const UINT                                  renderSubpassNum,
                const std::vector<VkSubpassDescription>&    subpassDescriptionList,
                const std::vector<VkAttachmentDescription>& attachmentDescriptionList);

            // Generate framebuffers
            BOOL createFramebuffers(
                std::vector<std::vector<Texture::VulkanTextureBase*>>* pFramebuffersTexturePtrList,
                const UINT                                             framebufferNum);

            // Context
            const Setting*                          m_pSetting;
            const VkDevice*                         m_pDevice;
            Mgr::CmdBufferMgr* const                m_pCmdBufferMgr;
            Mgr::DescriptorMgr* const               m_pDescriptorMgr;

            VDeleter<VkRenderPass>                  m_renderPass;

            VulkanGraphicsPipeline                  m_graphicsPipeline;

            // Resources
            const Scene::RenderScene*               m_pScene;

            std::vector<Buffer::VulkanFramebuffer>  m_framebufferList;

            std::vector<std::vector<Buffer::VulkanFramebuffer>> m_framebufferTable;
        };
    }
}