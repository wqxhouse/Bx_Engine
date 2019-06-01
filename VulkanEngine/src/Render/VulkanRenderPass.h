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
#include <Math/Shape.h>

#include "VulkanGraphicsPipeline.h"

namespace VulkanEngine
{
    namespace Render
    {
        enum RenderPassType
        {
            MAIN_RENDER_PASS = 0,
            GBUFFER_PASS     = 1,
            SHADOW_PASS      = 2,
            GENERAL_PASS
        };

        struct VulkanRenderProperties
        {
            Rectangle                 renderViewportRect;
            BOOL                      enableBlending;
            BOOL                      enableColor;
            std::vector<VkClearValue> sceneClearValue;
            BOOL                      enableDepth;
            VkClearValue              depthClearValue;
            BOOL                      enableStencil;
            VkClearValue              stencilClearValue;
        };

        struct VulkanRenderTargetFramebufferCreateData
        {
            UINT                     framebufferIndex;
            Buffer::VulkanAttachment attachment;
        };

        struct VulkanRenderTargetCreateData
        {
            BOOL                                                  isStore;
            UINT                                                  renderSubPassIndex;
            UINT                                                  bindingPoint;
            UINT                                                  attachmentIndex;
            UINT                                                  sampleNum;
            BX_FRAMEBUFFER_ATTACHMENT_LAYOUT                      layout;
            BOOL                                                  useStencil;
            BOOL                                                  isStoreStencil;
            std::vector<VulkanRenderTargetFramebufferCreateData>* pRenderTargetFramebufferCreateData;
        };

        struct VulkanRenderSubpassCreateData
        {
            // GraphicsPipeline create data
            VulkanSubpassGraphicsPipelineCreateData*    pSubpassGraphicsPipelineCreateData;
            std::vector<VulkanRenderTargetCreateData*>* pSubpassRenderTargetCreateDataRefList;
        };

        struct VulkanRenderpassCreateData
        {
            VulkanRenderProperties*                     pRenderProperties;
            UINT                                        framebufferNum;

            std::vector<VulkanRenderTargetCreateData>*  pRenderTargetCreateDataList;

            std::vector<VulkanRenderSubpassCreateData>* pSubpassCreateDataList;
            std::vector<VkSubpassDependency>*           pSubpassDependencyList;
        };

        class VulkanRenderPass
        {
        public:
            VulkanRenderPass(
                const Setting*      const pSetting,
                const VkDevice*     const pDevice,
                Mgr::CmdBufferMgr*  const pCmdBufferMgr,
                Mgr::DescriptorMgr* const pDescritorMgr,
                const Scene::RenderScene* pScene,
                const UINT                renderpassIndex,
                const RenderPassType      renderPassType);

            ~VulkanRenderPass();

            BOOL create(
                const VulkanRenderpassCreateData& renderPassCreateData);

            BOOL update(
                const float                                                 deltaTime,
                const std::vector<std::vector<VulkanDescriptorUpdateData>>& updateDataTable);

            BOOL draw();

            void clean();

            INLINE const Buffer::VulkanFramebuffer* GetFramebuffer(
                const UINT framebufferIndex) const
            {
                return &(m_framebufferList[framebufferIndex]);
            }

            INLINE const std::vector<UINT>& GetCmdBufferIndexList() const
            {
                return m_cmdBufferIndexList;
            }

            INLINE const VkRect2D& GetRenderViewport() const { return m_renderViewport; }

            INLINE const BOOL IsMainRenderPass() const { return (m_renderPassType == MAIN_RENDER_PASS);  }
            INLINE const BOOL IsGBufferPass()    const { return (m_renderPassType == GBUFFER_PASS);      }
            INLINE const BOOL IsShadowPass()     const { return (m_renderPassType == SHADOW_PASS);       }

            INLINE const BOOL IsColorEnabled()   const { return m_enableColor;                           }
            INLINE const BOOL IsDepthEnabled()   const { return m_enableDepth;                           }
            INLINE const BOOL IsStencilEnabled() const { return m_enableStencil;                         }


            std::vector<VulkanGraphicsPipeline>                 m_graphicsPipelineList;
        private:
            /*struct VulkanRenderPassUniformBufferUpdateData
            {
                Mgr::DescriptorUpdateInfo* pUpdateInfo;
                void*                      data;
                BOOL                       isUpdate;
            };*/

            // Create render targets for a single subpass
            BOOL createRenderTargets(
                IN  const std::vector<VulkanRenderTargetCreateData*>*    pRenderTargetsCreateDataRefList,
                IN  const std::vector<const VulkanDescriptorInfo*>*      pSubpassInputAttachmentDescriptorInfoPtrList,
                IN  const std::vector<const VulkanDescriptorInfo*>*      pSubpassResolveAttachmentDescriptorInfoPtrList,
                OUT VkSubpassDescription*                                pSubpassDescription,
                OUT std::vector<VkAttachmentDescription>*                pAttachmentDescriptionList,
                OUT std::vector<VkAttachmentReference>*                  pColorSubpassAttachmentRefList,
                OUT VkAttachmentReference*                               pDepthSubpassAttachmentRef,
                OUT std::vector<VkAttachmentReference>*                  pInputSubpassAttachmentRef,
                OUT std::vector<VkAttachmentReference>*                  pResolveSubpassAttachmentRef,
                OUT std::vector<std::vector<Buffer::VulkanAttachment*>>* pFramebuffersAttachmentsTable);

            // Create render pass
            BOOL createRenderPass(
                const UINT                                  renderSubpassNum,
                const std::vector<VkSubpassDescription>&    subpassDescriptionList,
                const std::vector<VkAttachmentDescription>& attachmentDescriptionList,
                const std::vector<VkSubpassDependency>*     pSubpassDependencyList);

            // Generate framebuffers
            BOOL createFramebuffers(
                std::vector<std::vector<Buffer::VulkanAttachment*>>* pFramebufferAttachmentPtrList,
                const UINT                                           framebufferNum);

            const UINT                                          m_renderpassIndex;
            const RenderPassType                                m_renderPassType;

            // Context
            const Setting*                                      m_pSetting;
            const VkDevice*                                     m_pDevice;
            Mgr::CmdBufferMgr* const                            m_pCmdBufferMgr;
            Mgr::DescriptorMgr* const                           m_pDescriptorMgr;

            VDeleter<VkRenderPass>                              m_renderPass;

            // Resources
            const Scene::RenderScene*                           m_pScene;

            std::vector<Buffer::VulkanFramebuffer>              m_framebufferList;

            std::vector<std::vector<Buffer::VulkanFramebuffer>> m_framebufferTable;

            VkRect2D                                            m_renderViewport;

            std::vector<VkClearValue>                           m_clearValueList;

            std::vector<UINT>                                   m_cmdBufferIndexList;

            std::vector<UINT>                                   m_descriptorSetIndexList;

            BOOL                                                m_enableColor;
            BOOL                                                m_enableDepth;
            BOOL                                                m_enableStencil;
        };
    }
}