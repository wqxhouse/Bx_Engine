//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2019
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include <Setting/Setting.h>

#include "../Core/VulkanPCH.h"
#include "../Context/CmdBufferMgr.h"
#include "../Context/DescriptorMgr.h"
#include "../Core/VulkanUtility.h"
#include "../Context/DescriptorMgr.h"
#include "../Buffer/VulkanVertexBuffer.h"
#include "../Buffer/VulkanIndexBuffer.h"
#include "../Buffer/VulkanUniformBufferDynamic.h"
#include "../Buffer/VulkanFramebuffer.h"
#include "../Shader/VulkanGraphicsShader.h"

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
            UINT                         setIndex;
            UINT                         bindingPoint;
            UINT                         uniformbufferNum;
            BX_SHADER_TYPE               shaderType;
            Buffer::VulkanUniformBuffer* pUniformBuffer;
        };

        struct VulkanRenderResources
        {
            UINT                                      vertexDescriptionBindingPoint;
            UINT                                      vertexBufferTexChannelNum;
            std::vector<VulkanVertexInputResource>*   pVertexInputResourceList;
            std::vector<VulkanUniformBufferResource>* pUniformBufferResourceList;
            std::vector<VulkanTextureResource>*       pTextureResouceList;
        };

        struct VulkanTextureResource
        {
            UINT                        setIndex;
            UINT                        bindingPoint;
            UINT                        textureNum;
            BX_SHADER_TYPE              shaderType;
            Texture::VulkanTextureBase* pTexture;
        };

        struct VulkanGraphicsPipelineCreateData
        {
            // Subpass Index
            UINT                    subpassIndex;

            // Properties
            VulkanRenderProperties* pProps;

            // Shader
            Shader::BxShaderMeta* pShaderMeta;

            // Resources (Vertex buffer, Uniform buffer, Texture, etc)
            VulkanRenderResources* pResource;
        };

        class VulkanGraphicsPipeline
        {
        public:
            VulkanGraphicsPipeline(
                const Setting* const pSetting);

            ~VulkanGraphicsPipeline();

            BOOL createGraphicsPipeline(
                const VulkanGraphicsPipelineCreateData& renderTargetsCreateData);

        private:
            const Setting*            m_pSetting;
            const VkDevice*           m_pDevice;

            VDeleter<VkPipelineLayout> m_graphicsPipelineLayout;

            Mgr::CmdBufferMgr* const  m_pCmdBufferMgr;
            Mgr::DescriptorMgr* const m_pDescriptorMgr;

            std::vector<Mgr::DescriptorUpdateInfo> m_uniformBufferDescriptorUpdateInfo;
            std::vector<Mgr::DescriptorUpdateInfo> m_textureDescriptorUpdateInfo;

            VkRect2D       m_renderViewport;
                          
            VkClearValue   m_clearColor;
            VkClearValue   m_depthColor;
            VkClearValue   m_stencilColor;

            BOOL           m_enableColor;
            BOOL           m_enableDepth;
            BOOL           m_enableStencil;
        };
    }
}