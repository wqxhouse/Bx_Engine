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
            // Graphics Pipelines create data
            std::vector<VulkanGraphicsPipelineCreateData>* pGraphicsPipelineCreateDataList;

            // Render Target create data
            UINT                                           renderSubPassNum;
            UINT                                           renderFramebufferNum;
            std::vector<VulkanRenderTargetCreateData>*     pRenderTargetCreateDataList;
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
                const UINT                                       renderFramebufferNum);

            // Context
            const Setting*                          m_pSetting;
            const VkDevice*                         m_pDevice;
            Mgr::CmdBufferMgr* const                m_pCmdBufferMgr;
            Mgr::DescriptorMgr* const               m_pDescriptorMgr;

            VDeleter<VkRenderPass>                  m_renderPass;
            //VDeleter<VkPipeline>                    m_graphicsPipeline;

            // Resources
            const Scene::RenderScene*               m_pScene;

            //Shader::VulkanGraphicsShader            m_shader;

            std::vector<Buffer::VulkanFramebuffer>  m_framebufferList;

            /*std::vector<VulkanVertexInputResource>* m_pVertexInputResourceList;*/

            std::vector<Mgr::DescriptorUpdateInfo>  m_uniformBufferDescriptorUpdateInfo;
            //std::vector<Mgr::DescriptorUpdateInfo>  m_textureDescriptorUpdateInfo;

            //VDeleter<VkPipelineLayout>              m_graphicsPipelineLayout;

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