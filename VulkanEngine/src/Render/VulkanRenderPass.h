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
#include "../Buffer/VulkanUniformBufferDynamic.h"
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
            BOOL                   enableDepth;
            BOOL                   enableStencil;
            std::vector<Rectangle> viewportRects;
            std::vector<Rectangle> scissorRects;
            VkClearValue           sceneClearValue;
            VkClearValue           depthClearValue;
            VkClearValue           stencilClearValue;
            Rectangle              renderViewportRect;
        };

        struct VulkanVertexInputResource
        {
            std::unique_ptr<Buffer::VulkanVertexBuffer> pVertexBuffer;
            std::unique_ptr<Buffer::VulkanIndexBuffer>  pIndexBuffer;
        };

        struct VulkanUniformBufferResource
        {
            UINT                         bindingPoint;
            UINT                         uniformbufferNum;
            BX_SHADER_TYPE               shaderType;
            Buffer::VulkanUniformBuffer* pUniformBuffer;
        };

        struct VulkanTextureResource
        {
            UINT                        bindingPoint;
            UINT                        textureNum;
            BX_SHADER_TYPE              shaderType;
            Texture::VulkanTextureBase* pTexture;
        };

        struct VulkanRenderResources
        {
            UINT                                      vertexDescriptionBindingPoint;
            UINT                                      vertexBufferTexChannelNum;
            std::vector<VulkanVertexInputResource>*   pVertexInputResourceList;
            std::vector<VulkanUniformBufferResource>* pUniformBufferResourceList;
            std::vector<VulkanTextureResource>*       pTextureResouceList;
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

            BOOL update(const float deltaTime);

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

            // Create VkRenderPass and generate framebuffers
            BOOL createRenderTargets(
                const std::vector<VulkanRenderTargetCreateData>* pRenderTargetsCreateDataList,
                const UINT                                       renderSubpassNum,
                UINT                                             renderFramebufferNum);

            BOOL createGraphicsPipeline(
                VulkanRenderProperties* const pProps,
                Shader::BxShaderMeta*   const pShaderMeta,
                VulkanRenderResources*  const pResource);

            // Context
            const Setting*                          m_pSetting;
            const VkDevice*                         m_pDevice;
            Mgr::CmdBufferMgr* const                m_pCmdBufferMgr;
            Mgr::DescriptorMgr* const               m_pDescriptorMgr;

            VDeleter<VkRenderPass>                  m_renderPass;
            VDeleter<VkPipeline>                    m_graphicsPipeline;

            // Resources
            const Scene::RenderScene*               m_pScene;

            Shader::VulkanGraphicsShader            m_shader;

            std::vector<Buffer::VulkanFramebuffer>  m_framebufferList;

            std::vector<VulkanVertexInputResource>* m_pVertexInputResourceList;

            std::vector<Mgr::DescriptorUpdateInfo>  m_uniformBufferDescriptorUpdateInfo;
            std::vector<Mgr::DescriptorUpdateInfo>  m_textureDescriptorUpdateInfo;

            VDeleter<VkPipelineLayout>              m_graphicsPipelineLayout;

            VkRect2D                                m_renderViewport;

            VkClearValue                            m_clearColor;
            VkClearValue                            m_depthColor;
            VkClearValue                            m_stencilColor;

            BOOL                                    m_enableColor;
            BOOL                                    m_enableDepth;
            BOOL                                    m_enableStencil;
        };
    }
}