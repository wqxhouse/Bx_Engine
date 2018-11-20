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

            // TODO: Indexed draws

            inline void endRenderPass()
            {
                vkCmdEndRenderPass(m_cmdBuffer);
                m_cmdStageFlags.render = 0;
            }

            inline BOOL endCmdBuffer()
            {
                BOOL result = BX_SUCCESS;

                VkResult endCmdBufferResult = vkEndCommandBuffer(m_cmdBuffer);
                result = Utility::VulkanUtility::GetBxStatus(endCmdBufferResult);

                assert(result == BX_SUCCESS);

                m_cmdStageFlags.begin = 0;

                return result;
            }

            inline const VkCommandBuffer& GetCmdBuffer() const
            {
                return m_cmdBuffer;
            }

            inline const VkCommandBuffer* GetCmdBufferPtr()
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
                    UINT begin : 1;  //< Command buffer begin record commands
                    UINT render : 1;  //< Command buffer begin render pass
                    UINT reserve : 30; //< Reserve
                };
            } m_cmdStageFlags;
        };
    }
}
