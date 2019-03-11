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
        struct VulkanGraphicsPipelineRenderTargetProperties
        {
            BOOL enableBlend;
        };

        struct VulkanGraphicsPipelineProperties
        {
            PolyMode                                                   polyMode;
            CullMode                                                   cullMode;
            std::vector<Rectangle>                                     viewportRects;
            std::vector<Rectangle>                                     scissorRects;
            std::vector<VulkanGraphicsPipelineRenderTargetProperties>* pRenderTargetsProps;
        };

        struct VulkanVertexInputResource
        {
            std::unique_ptr<Buffer::VulkanVertexBuffer> pVertexBuffer;
            std::unique_ptr<Buffer::VulkanIndexBuffer>  pIndexBuffer;
        };

        struct VulkanDescriptorInfo
        {
            UINT           setIndex;
            UINT           bindingPoint;
            UINT           attachmentIndex;
            BX_SHADER_TYPE shaderType;
        };

        struct VulkanUniformBufferResource : public VulkanDescriptorInfo
        {
            UINT                         uniformbufferNum;
            Buffer::VulkanUniformBuffer* pUniformBuffer;
        };

        struct VulkanTextureResource : public VulkanDescriptorInfo
        {
            UINT                        textureNum;
            Texture::VulkanTextureBase* pTexture;
        };

        struct VulkanDescriptorResources
        {
            UINT                                      descriptorSetIndex;
            std::vector<VulkanUniformBufferResource>* pUniformBufferResourceList;
            std::vector<VulkanTextureResource>*       pTextureResouceList;
            std::vector<VulkanTextureResource>*       pInputAttachmentList;
            std::vector<VulkanTextureResource>*       pResolveAttachmentList;
        };

        struct VulkanRenderResources
        {
            UINT                                    vertexDescriptionBindingPoint;
            std::vector<VulkanVertexInputResource>* pVertexInputResourceList;
            std::vector<VulkanDescriptorResources>* pDescriptorResourceList;
        };

        struct VulkanGraphicsPipelineCreateData
        {
            UINT                                            subpassIndex;

            BOOL                                            enableColor;
            BOOL                                            enableDepth;
            BOOL                                            enableStencil;

            // Properties
            VulkanGraphicsPipelineProperties*               pProps;

            // Shader
            Shader::BxShaderMeta*                           pShaderMeta;

            // Input layouts
            VkVertexInputBindingDescription*                pVertexInputBindingDescription;
            std::vector<VkVertexInputAttributeDescription>* pVertexInputAttributeDescriptionList;

            // Resources (Vertex buffer, Uniform buffer, Texture, etc)
            VulkanRenderResources*                          pResource;

            // Is the pipeline used for rendering scene
            BOOL                                            isScenePipeline;
        };

        struct VulkanSubpassGraphicsPipelineCreateData
        {
            // Subpass Index
            UINT                                            subpassIndex;

            // Properties
            VulkanGraphicsPipelineProperties*               pProps;

            // Shader
            Shader::BxShaderMeta*                           pShaderMeta;

            // Input layouts
            VkVertexInputBindingDescription*                pVertexInputBindingDescription;
            std::vector<VkVertexInputAttributeDescription>* pVertexInputAttributeDescriptionList;

            // Resources (Vertex buffer, Uniform buffer, Texture, etc)
            VulkanRenderResources*                          pResource;

            // Is the pipeline used for rendering scene
            BOOL                                            isScenePipeline;
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

            INLINE const BOOL IsScenePipeline() const
            {
                return m_isScenePipeline;
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
            std::vector<Mgr::DescriptorUpdateInfo>  m_inputAttachmentUpdateInfo;

            std::vector<VulkanVertexInputResource>* m_pVertexInputResourceList;

            BOOL                                    m_isScenePipeline;
        };
    }
}
