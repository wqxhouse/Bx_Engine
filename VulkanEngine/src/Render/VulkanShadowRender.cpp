//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2019
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "VulkanShadowRender.h"

#include "VulkanRenderPass.h"

namespace VulkanEngine
{
    namespace Render
    {
        VulkanShadowRender::VulkanShadowRender()
        {
        }

        VulkanShadowRender::~VulkanShadowRender()
        {
        }

        BOOL VulkanShadowRender::initializeShadowRender()
        {BOOL status = BX_SUCCESS;

            //  Initialize graphics pipeline properties
            VulkanRenderProperties props = {};
            props.enableBlending         = FALSE;
            props.enableColor            = FALSE;
            props.enableDepth            = TRUE;
            props.enableStencil          = FALSE;

            props.depthClearValue = { 1.0f, 0.0f };

            props.renderViewportRect =
            {
                0.0f, static_cast<float>(m_pSetting->resolution.width),
                static_cast<float>(m_pSetting->resolution.height), 0.0f
            };

            Shader::BxShaderMeta shadowmapShaderMeta          = {};
            shadowmapShaderMeta.vertexShaderInfo.shaderFile   = "ShadowMap.vert.spv";
            shadowmapShaderMeta.fragmentShaderInfo.shaderFile = "ShadowMap.frag.spv";

            /// Initialize render pass
            const UINT sampleNum = m_pSetting->m_graphicsSetting.antialasing;

            Texture::VulkanTexture2D* pShadowMapTexture =
                m_pTextureMgr->createTexture2DRenderTarget(
                    m_pSetting->resolution.width,
                    m_pSetting->resolution.height,
                    sampleNum,
                    BX_FORMAT_DEPTH32,
                    BX_TEXTURE_USAGE_VULKAN_NONE,
                    ((m_pSetting->m_graphicsSetting.antialasing == AA_NONE) ? FALSE : TRUE));

            Texture::VulkanTexture2D* pShadowMapResolveTexture = NULL;

            std::vector<VulkanRenderTargetCreateDescriptor> shadowMapDescriptors =
            {
                { TRUE, 0, 0, sampleNum, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_DEPTH_STENCIL, FALSE, FALSE }
            };

            if (m_pSetting->m_graphicsSetting.antialasing != AA_NONE)
            {
                shadowMapDescriptors.push_back(
                    { TRUE, 0, 1, sampleNum, BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_DEPTH_STENCIL, FALSE, FALSE });

                pShadowMapResolveTexture =
                    m_pTextureMgr->createTexture2DRenderTarget(
                        m_pSetting->resolution.width,
                        m_pSetting->resolution.height,
                        1,
                        BX_FORMAT_DEPTH32,
                        BX_TEXTURE_USAGE_VULKAN_NONE,
                        FALSE);
            }

            VulkanRenderTargetCreateData shadowPassRTCreateData = {};
            shadowPassRTCreateData.renderSubPassIndex           = 0;
            shadowPassRTCreateData.bindingPoint                 = 0;
            shadowPassRTCreateData.attachmentIndex              = 0;
            shadowPassRTCreateData.sampleNum                    = sampleNum;
            shadowPassRTCreateData.isStore                      = TRUE;
            shadowPassRTCreateData.layout                       = BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_DEPTH_STENCIL;
            shadowPassRTCreateData.useStencil                   = FALSE;
            shadowPassRTCreateData.isStoreStencil               = FALSE;

            std::vector<VulkanRenderTargetFramebufferCreateData> shadowPassFboCreateDataList(1);
            shadowPassFboCreateDataList[0].framebufferIndex = 0;
            shadowPassFboCreateDataList[0].attachment       = { sampleNum, pShadowMapTexture };

            shadowPassRTCreateData.pRenderTargetFramebufferCreateData = &shadowPassFboCreateDataList;

            // Initialize vertex input for render pass
            VulkanRenderResources renderSources         = {};
            renderSources.vertexDescriptionBindingPoint = 0;
            renderSources.pVertexInputResourceList      = &m_mainSceneVertexInputResourceList;

            const UINT shadowPassUboSetIndex   =
                ((m_pSetting->m_graphicsSetting.renderingMethod == FORWARD_RENDERING) ||
                 (m_pSetting->m_graphicsSetting.renderingMethod == FORWARD_PLUS_RENDERING) ||
                 (m_pSetting->m_graphicsSetting.renderingMethod == FORWARD_CLUSTERED_RENDERING) == TRUE) ? 1 : 2;
            const UINT shadowPassTransUboIndex = 0;

            std::vector<VulkanUniformBufferResource> shadowPassUniformBufferResourceList =
            {
                createTransMatrixUniformBufferResource(shadowPassUboSetIndex, shadowPassTransUboIndex)
            };

            VulkanDescriptorResources shadowPassDescriptorResources  = {};
            shadowPassDescriptorResources.descriptorSetIndex         = shadowPassUboSetIndex;
            shadowPassDescriptorResources.pUniformBufferResourceList = &shadowPassUniformBufferResourceList;

            std::vector<VulkanTextureResource> resolveShadowMap(1);
            resolveShadowMap[0].setIndex     = shadowPassUboSetIndex;
            resolveShadowMap[0].bindingPoint = 0;

            resolveShadowMap[0].pTextureList.push_back(pShadowMapResolveTexture);

            if (m_pSetting->m_graphicsSetting.antialasing != Antialasing::AA_NONE)
            {
                shadowPassDescriptorResources.pResolveAttachmentList = &resolveShadowMap;
            }

            std::vector<VulkanDescriptorResources> descriptorResourceList = { shadowPassDescriptorResources };

            renderSources.pDescriptorResourceList = &descriptorResourceList;

            // Create render pass create data
            std::vector<VulkanGraphicsPipelineRenderTargetProperties> renderTargetsProps(1);
            renderTargetsProps[0].enableBlend = FALSE;

            VkVertexInputBindingDescription vertexInputBindingDescription =
                Buffer::VulkanVertexBuffer::CreateDescription(0, sizeof(Object::Model::Vertex), BX_VERTEX_INPUT_RATE_VERTEX);

            std::vector<VkVertexInputAttributeDescription> vertexAttributeDescription =
                Buffer::VulkanVertexBuffer::CreateAttributeDescriptions(
                    Buffer::VulkanVertexBuffer::GenSingleTextureChannelMeshAttributeListCreateData(0));

            VulkanGraphicsPipelineProperties subpassGraphicsPipelineProperties     = {};
            subpassGraphicsPipelineProperties.cullMode                             = CULLMODE_BACK;
            subpassGraphicsPipelineProperties.polyMode                             = POLYMODE_FILL;
            subpassGraphicsPipelineProperties.samples                              = m_pSetting->m_graphicsSetting.antialasing;
            subpassGraphicsPipelineProperties.viewportRects                        = { props.renderViewportRect };
            subpassGraphicsPipelineProperties.scissorRects                         = { props.renderViewportRect };
            subpassGraphicsPipelineProperties.pRenderTargetsProps                  = &renderTargetsProps;

            VulkanSubpassGraphicsPipelineCreateData subpassGraphicsPipelineCreateData = {};
            subpassGraphicsPipelineCreateData.subpassIndex                            = 0;
            subpassGraphicsPipelineCreateData.pProps                                  = &subpassGraphicsPipelineProperties;
            subpassGraphicsPipelineCreateData.pShaderMeta                             = &shadowmapShaderMeta;
            subpassGraphicsPipelineCreateData.pResource                               = &renderSources;
            subpassGraphicsPipelineCreateData.isScenePipeline                         = TRUE;
            subpassGraphicsPipelineCreateData.pVertexInputBindingDescription          = &vertexInputBindingDescription;
            subpassGraphicsPipelineCreateData.pVertexInputAttributeDescriptionList    = &vertexAttributeDescription;

            std::vector<VulkanRenderTargetCreateData*> shadowPasspRTCreateDataList = { &shadowPassRTCreateData };

            std::vector<VulkanRenderSubpassCreateData> renderSubpassCreateDataList(1);
            renderSubpassCreateDataList[0].pSubpassGraphicsPipelineCreateData    = &subpassGraphicsPipelineCreateData;
            renderSubpassCreateDataList[0].pSubpassRenderTargetCreateDataRefList = &shadowPasspRTCreateDataList;

            std::vector<VulkanRenderTargetCreateData> shadowPassRTCreateDataList = { shadowPassRTCreateData };

            VulkanRenderpassCreateData renderPassCreateData                      = {};
            renderPassCreateData.pRenderProperties                               = &props;
            renderPassCreateData.pSubpassCreateDataList                          = &renderSubpassCreateDataList;
            renderPassCreateData.framebufferNum                                  = 1;
            renderPassCreateData.pRenderTargetCreateDataList                     = &shadowPassRTCreateDataList;

            status = AddRenderPass(BX_RENDER_PASS_PRE_RENDER, SHADOW_PASS, renderPassCreateData);

            assert(status == BX_SUCCESS);

            return status;
        }
    }
}