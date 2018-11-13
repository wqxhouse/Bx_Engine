#pragma once

#include "../Core/VulkanPCH.h"
#include "../Core/VulkanUtility.h"

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
    }

    inline BOOL endCmdBuffer()
    {
        BOOL result = BX_SUCCESS;

        VkResult endCmdBufferResult = vkEndCommandBuffer(m_cmdBuffer);
        result = VulkanUtility::GetBxStatus(endCmdBufferResult);

        return result;
    }

    inline const VkCommandBuffer& GetCmdBuffer() const
    {
        return m_cmdBuffer;
    }

    inline VkCommandBuffer* GetCmdBufferPtr()
    {
        return &m_cmdBuffer;
    }

private:
    BX_COMMAND_BUFFER_TYPE  m_cmdBufferType;
    BX_COMMAND_BUFFER_LEVLE m_cmdBufferLevel;
    VkCommandBuffer         m_cmdBuffer;
};