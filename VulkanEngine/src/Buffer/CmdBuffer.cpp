#include "CmdBuffer.h"

CmdBuffer::CmdBuffer(
    const BX_COMMAND_BUFFER_TYPE  cmdBufferType,
    const BX_COMMAND_BUFFER_LEVLE bufferLevel,
    const VkCommandBuffer&        cmdBuffer)
    : m_cmdBufferType(cmdBufferType),
      m_cmdBufferLevel(bufferLevel),
      m_cmdBuffer(cmdBuffer)
{
}

CmdBuffer::~CmdBuffer()
{
}

BOOL CmdBuffer::beginCmdBuffer(
    const BOOL reuse)
{
    BOOL result = BX_SUCCESS;

    VkCommandBufferUsageFlags cmdBufferUsageFlags = ((reuse == TRUE) ?
                                                      VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT :
                                                      VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
    cmdBufferBeginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufferBeginInfo.flags            = cmdBufferUsageFlags;
    cmdBufferBeginInfo.pInheritanceInfo = NULL;

    VkResult beginCmdBufferResult = vkBeginCommandBuffer(m_cmdBuffer, &cmdBufferBeginInfo);
    result = VulkanUtility::GetBxStatus(beginCmdBufferResult);

    assert(result == BX_SUCCESS);

    return result;
}

BOOL CmdBuffer::beginCmdBuffer(
    const BOOL                            reuse,
    const VkCommandBufferInheritanceInfo& cmdBufferInheritanceInfo)
{
    BOOL result = BX_SUCCESS;

    VkCommandBufferUsageFlags cmdBufferUsageFlags = ((reuse == TRUE) ?
        VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT :
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    cmdBufferUsageFlags |= ((m_cmdBufferLevel == BX_INDIRECT_COMMAND_BUFFER) ?
        VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT : 0);

    VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
    cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufferBeginInfo.flags = cmdBufferUsageFlags;
    cmdBufferBeginInfo.pInheritanceInfo = NULL;

    if (m_cmdBufferLevel == BX_INDIRECT_COMMAND_BUFFER)
    {
        cmdBufferBeginInfo.pInheritanceInfo = &cmdBufferInheritanceInfo;
    }
    else
    {
        cmdBufferBeginInfo.pInheritanceInfo = NULL;
    }

    VkResult beginCmdBufferResult = vkBeginCommandBuffer(m_cmdBuffer, &cmdBufferBeginInfo);
    result = VulkanUtility::GetBxStatus(beginCmdBufferResult);

    assert(result == BX_SUCCESS);

    return result;
}

void CmdBuffer::beginRenderPass(
    const VkRenderPass&              renderPass,
    const VkFramebuffer&             framebuffer,
    const VkRect2D&                  renderArea,
    const std::vector<VkClearValue>& clearColors)
{
    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass      = renderPass;
    renderPassBeginInfo.framebuffer     = framebuffer;
    renderPassBeginInfo.renderArea      = renderArea;
    renderPassBeginInfo.clearValueCount = static_cast<UINT>(clearColors.size());
    renderPassBeginInfo.pClearValues    = clearColors.data();

    VkSubpassContents subpassContents = (m_cmdBufferType == BX_DIRECT_COMMAND_BUFFER) ?
                                         VK_SUBPASS_CONTENTS_INLINE :
                                         VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS;

    vkCmdBeginRenderPass(m_cmdBuffer, &renderPassBeginInfo, subpassContents);
}

void CmdBuffer::cmdDrawArrays(
    const VkPipeline& graphicsPipeline,
    const UINT        vertexCount,
    const UINT        vertexOffset)
{
    vkCmdBindPipeline(m_cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
    vkCmdDraw(m_cmdBuffer, vertexCount, 1, vertexOffset, 0);
}

void CmdBuffer::cmdDrawArraysInstanced(
    const VkPipeline& graphicsPipeline,
    const UINT        vertexCount,
    const UINT        instanceCount,
    const UINT        vertexOffset,
    const UINT        instanceOffset)
{
    vkCmdBindPipeline(m_cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
    vkCmdDraw(m_cmdBuffer, vertexCount, instanceCount, vertexOffset, instanceOffset);
}
