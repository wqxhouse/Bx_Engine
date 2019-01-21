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

#include "../Core/VulkanPCH.h"
#include "../Context/CmdBufferMgr.h"
#include "../Context/DescriptorMgr.h"
#include "../Shader/VulkanGraphicsShader.h"
#include "../Buffer/VulkanVertexBuffer.h"
#include "../Buffer/VulkanIndexBuffer.h"
#include "../Buffer/VulkanUniformBuffer.h"
#include "../Buffer/VulkanFramebuffer.h"

namespace VulkanEngine
{
    namespace Render
    {
        struct VulkanRenderProperties
        {
            PolyMode               polyMode;
            CullMode               cullMode;
            BOOL                   enableBlending;
            std::vector<Rectangle> viewportRects;
            std::vector<Rectangle> scissorRects;
            VkClearValue           sceneClearValue;
            Rectangle              renderViewportRect;
        };

        struct VulkanVertexInputResources
        {
            Buffer::VulkanVertexBuffer* pVertexBuffer;
            Buffer::VulkanIndexBuffer*  pIndexBuffer;
        };

        struct VulkanUniformBufferResources
        {
            UINT                         bindingPoint;
            Buffer::VulkanUniformBuffer* pUniformBuffer;
        };

        struct VulkanTextureResources
        {
            UINT                        bindingPoint;
            Texture::VulkanTextureBase* pTexture;
        };

        struct VulkanRenderResources
        {
            UINT                                        vertexDescriptionBindingPoint;
            UINT                                        vertexBufferTexChannelNum;
            std::vector<VulkanVertexInputResources>*    pVertexInputResourceList;
            std::vector<VulkanUniformBufferResources*>* ppUniformBufferResourceList;
            std::vector<VulkanTextureResources*>*       ppTextureResouces;
        };

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

        struct VulkanRenderPassCreateData
        {
            // Properties
            VulkanRenderProperties*                    pProps;

            // Shader
            Shader::BxShaderMeta*                      pShaderMeta;

            // Input
            VulkanRenderResources*                     pResource;

            // Output
            UINT                                       renderSubPassNum;
            UINT                                       renderFramebufferNum;
            std::vector<VulkanRenderTargetCreateData>* pRenderTargetCreateDataList;
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

            BOOL draw();

            void clean();

            INLINE const Buffer::VulkanFramebuffer* GetFramebuffer(
                const UINT framebufferIndex) const
            {
                return &(m_framebufferList[framebufferIndex]);
            }

        private:
            // Create VkRenderPass and generate framebuffers
            BOOL createRenderTargets(
                const std::vector<VulkanRenderTargetCreateData>* pRenderTargetsCreateDataList,
                const UINT                                       renderSubpassNum,
                UINT                                             renderFramebufferNum);

            BOOL createGraphicsPipeline(
                VulkanRenderProperties* const pProps,
                Shader::BxShaderMeta*   const pShaderMeta,
                VulkanRenderResources*  const pResource);

            const Setting*            m_pSetting;
            const VkDevice*           m_pDevice;
            Mgr::CmdBufferMgr* const  m_pCmdBufferMgr;
            Mgr::DescriptorMgr* const m_pDescriptorMgr;

            VDeleter<VkRenderPass>    m_renderPass;
            VDeleter<VkPipeline>      m_graphicsPipeline;

            const Scene::RenderScene* m_pScene;

            Shader::VulkanGraphicsShader m_shader;

            std::vector<Buffer::VulkanFramebuffer> m_framebufferList;

            std::vector<VulkanVertexInputResources>* m_pVertexInputResourceList;

            VkRect2D      m_renderViewport;
            VkClearValue* m_pClearColor;
        };
    }
}