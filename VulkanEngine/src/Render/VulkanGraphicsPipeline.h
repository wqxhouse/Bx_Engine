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
        struct VulkanGraphicsPipelineProperties
        {
            PolyMode               polyMode;
            CullMode               cullMode;
            std::vector<Rectangle> viewportRects;
            std::vector<Rectangle> scissorRects;
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

        struct VulkanTextureResource
        {
            UINT                        setIndex;
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

        struct VulkanGraphicsPipelineCreateData
        {
            size_t                            renderTargetNum;

            BOOL                              enableColor;
            BOOL                              enableDepth;
            BOOL                              enableStencil;

            // Properties
            VulkanGraphicsPipelineProperties* pProps;

            // Shader
            Shader::BxShaderMeta*             pShaderMeta;

            // Resources (Vertex buffer, Uniform buffer, Texture, etc)
            VulkanRenderResources*            pResource;

        };

        struct VulkanSubpassGraphicsPipelineCreateData
        {
            // Subpass Index
            UINT                              subpassIndex;

            // Properties
            VulkanGraphicsPipelineProperties* pProps;

            // Shader
            Shader::BxShaderMeta*             pShaderMeta;

            // Resources (Vertex buffer, Uniform buffer, Texture, etc)
            VulkanRenderResources*            pResource;
        };

        struct VulkanDescriptorUpdateData
        {
            UINT  descriptorIndex;
            void* pData;
            BOOL  isDataUpdated;
        };

        class VulkanGraphicsPipeline
        {
        public:
            VulkanGraphicsPipeline(
                const Setting*      const pSetting,
                const VkDevice*     const pDevice,
                const VkRenderPass* const pRenderPass,
                Mgr::CmdBufferMgr*  const pCmdBufferMgr,
                Mgr::DescriptorMgr* const pDescritorMgr);

            ~VulkanGraphicsPipeline();

            BOOL createGraphicsPipeline(
                VulkanGraphicsPipelineCreateData* pGraphicsPipelineCreateData);

            BOOL update(
                const float                                    deltaTime,
                const std::vector<VulkanDescriptorUpdateData>& updateDataList);

            INLINE const VkPipeline GetGraphicsPipelineHandle() const
            {
                return m_graphicsPipeline;
            }

            INLINE const VkPipelineLayout GetGraphicsPipelineLayout() const
            {
                return m_graphicsPipelineLayout;
            }

            INLINE const std::vector<Mgr::DescriptorUpdateInfo>& GetUniformBufferDescUpdateInfo() const
            {
                return m_uniformBufferDescriptorUpdateInfo;
            }

            INLINE const std::vector<Mgr::DescriptorUpdateInfo>& GetTextureDescUpdateInfo() const
            {
                return m_textureDescriptorUpdateInfo;
            }

            INLINE const std::vector<VulkanVertexInputResource>* GetVertexInputResourceList() const
            {
                return m_pVertexInputResourceList;
            }

        private:
            const Setting*                          m_pSetting;
            const VkDevice*                         m_pDevice;
            const VkRenderPass* const               m_pRenderPass;

            VDeleter<VkPipeline>                    m_graphicsPipeline;
            VDeleter<VkPipelineLayout>              m_graphicsPipelineLayout;

            Mgr::CmdBufferMgr* const                m_pCmdBufferMgr;
            Mgr::DescriptorMgr* const               m_pDescriptorMgr;

            Shader::VulkanGraphicsShader            m_shader;

            std::vector<Mgr::DescriptorUpdateInfo>  m_uniformBufferDescriptorUpdateInfo;
            std::vector<Mgr::DescriptorUpdateInfo>  m_textureDescriptorUpdateInfo;

            std::vector<VulkanVertexInputResource>* m_pVertexInputResourceList;
        };
    }
}
