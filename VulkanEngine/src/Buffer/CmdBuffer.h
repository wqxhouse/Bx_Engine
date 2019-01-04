//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../Core/VulkanPCH.h"
#include "../Core/VulkanUtility.h"
#include "../Buffer/BufferInfo.h"

namespace VulkanEngine
{
    namespace Buffer
    {
        class CmdBuffer
        {
        public:
            CmdBuffer(
                const BX_COMMAND_BUFFER_TYPE  cmdBufferType,
                const BX_COMMAND_BUFFER_LEVLE bufferLevel,
                const VkCommandBuffer&        cmdBuffer);

            ~CmdBuffer();

            static CmdBuffer CreateCmdBuffer(
                const VkDevice* const        pDevice,
                const BxCmdBufferCreateInfo& cmdBufferCreateInfo);

            BOOL beginCmdBuffer(
                const BOOL reuse);

            BOOL beginCmdBuffer(
                const BOOL                            reuse,
                const VkCommandBufferInheritanceInfo& cmdBufferInheritanceInfo);

            void beginRenderPass(
                const VkRenderPass&              renderPass,
                const VkFramebuffer&             framebuffer,
                const VkRect2D&                  renderArea,
                const std::vector<VkClearValue>& clearColors);

            void cmdBindVertexBuffers(
                const std::vector<VkBuffer>&     vertexBuffers,
                const std::vector<VkDeviceSize>& offsets);

            void cmdBindIndexBuffers(
                const VkBuffer&     indexBuffer,
                const VkDeviceSize& offset,
                const BX_INDEX_TYPE indexType);

            void cmdBindDescriptorSets(
                const VkPipelineLayout&             pipelineLayout,
                const std::vector<VkDescriptorSet>& descriptorSets);

            void cmdCopyBuffer(
                const VkBuffer&         srcBuffer,
                const VkBuffer&         dstBuffer,
                const BxBufferCopyInfo& copyInfo);

            void cmdCopyBufferToImage(
                const VkBuffer&                             srcBuffer,
                const VkImage&                              dstImage,
                const std::vector<BxBufferToImageCopyInfo>& copyInfo);

            void cmdImageLayoutTransition(
                const VkImage&                             image,
                const std::vector<BxLayoutTransitionInfo>& layoutTransInfoList);

            void cmdDrawArrays(
                const VkPipeline& graphicsPipeline,
                const UINT        vertexCount,
                const UINT        vertexOffset);

            void cmdDrawArraysInstanced(
                const VkPipeline& graphicsPipeline,
                const UINT        vertexCount,
                const UINT        instanceCount,
                const UINT        vertexOffset,
                const UINT        instanceOffset);

            void cmdDrawElements(
                const VkPipeline& graphicsPipeline,
                const UINT        indexCount,
                const UINT        indexOffset,
                const UINT        vertexOffset);

            void cmdDrawElementsInstanced(
                const VkPipeline& graphicsPipeline,
                const UINT        indexCount,
                const UINT        instanceCount,
                const UINT        indexOffset,
                const UINT        vertexOffset,
                const UINT        instanceOffset);

            INLINE void endRenderPass()
            {
                vkCmdEndRenderPass(m_cmdBuffer);
                m_cmdStageFlags.render = 0;
            }

            INLINE BOOL endCmdBuffer()
            {
                BOOL result = BX_SUCCESS;

                VkResult endCmdBufferResult = vkEndCommandBuffer(m_cmdBuffer);
                result = Utility::VulkanUtility::GetBxStatus(endCmdBufferResult);

                assert(result == BX_SUCCESS);

                m_cmdStageFlags.begin = 0;

                return result;
            }

            INLINE const VkCommandBuffer& GetCmdBuffer() const
            {
                return m_cmdBuffer;
            }

            INLINE const VkCommandBuffer* GetCmdBufferPtr()
            {
                return &m_cmdBuffer;
            }

        private:
            BX_COMMAND_BUFFER_TYPE  m_cmdBufferType;
            BX_COMMAND_BUFFER_LEVLE m_cmdBufferLevel;
            VkCommandBuffer         m_cmdBuffer;

            union CmdStageFlags
            {
                UINT value;
                struct
                {
                    UINT begin   : 1;  //< Command buffer begin record commands
                    UINT render  : 1;  //< Command buffer begin render pass
                    UINT reserve : 30; //< Reserve
                };
            } m_cmdStageFlags;
        };
    }
}
