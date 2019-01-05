//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "CmdBuffer.h"
#include "VulkanBuffer.h"

namespace VulkanEngine
{
    namespace Buffer
    {
        CmdBuffer::CmdBuffer(
            const BX_COMMAND_BUFFER_TYPE  cmdBufferType,
            const BX_COMMAND_BUFFER_LEVLE bufferLevel,
            const VkCommandBuffer&        cmdBuffer)
            : m_cmdBufferType(cmdBufferType),
              m_cmdBufferLevel(bufferLevel),
              m_cmdBuffer(cmdBuffer)
        {
            m_cmdStageFlags.value = 0;
        }

        CmdBuffer::~CmdBuffer()
        {
        }

        CmdBuffer CmdBuffer::CreateCmdBuffer(
            const VkDevice* const        pDevice,
            const BxCmdBufferCreateInfo& cmdBufferCreateInfo)
        {
            VkCommandBuffer vkCmdBuffer;

            VkCommandBufferAllocateInfo cmdBufferAllocInfo = {};
            cmdBufferAllocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmdBufferAllocInfo.commandPool                 = *cmdBufferCreateInfo.pCommandPool;
            cmdBufferAllocInfo.level                       =
                Utility::VulkanUtility::GetVkCmdBufferLevel(cmdBufferCreateInfo.bufferLevel);
            cmdBufferAllocInfo.commandBufferCount          = 1;

            VkResult createCmdBufferResult = vkAllocateCommandBuffers(*pDevice, &cmdBufferAllocInfo, &vkCmdBuffer);

            CmdBuffer cmdBuffer(cmdBufferCreateInfo.cmdBufferType,
                                cmdBufferCreateInfo.bufferLevel,
                                vkCmdBuffer);

            return cmdBuffer;
        }

        BOOL CmdBuffer::beginCmdBuffer(
            const BOOL reuse)
        {
            BOOL result = BX_SUCCESS;

            VkCommandBufferUsageFlags cmdBufferUsageFlags = ((reuse == TRUE) ?
                VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT :
                VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

            VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
            cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            cmdBufferBeginInfo.flags = cmdBufferUsageFlags;
            cmdBufferBeginInfo.pInheritanceInfo = NULL;

            VkResult beginCmdBufferResult = vkBeginCommandBuffer(m_cmdBuffer, &cmdBufferBeginInfo);
            result = Utility::VulkanUtility::GetBxStatus(beginCmdBufferResult);

            assert(result == BX_SUCCESS);

            m_cmdStageFlags.begin = 1;

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
            result = Utility::VulkanUtility::GetBxStatus(beginCmdBufferResult);

            assert(result == BX_SUCCESS);

            m_cmdStageFlags.begin = 1;

            return result;
        }

        void CmdBuffer::beginRenderPass(
            const VkRenderPass&              renderPass,
            const VkFramebuffer&             framebuffer,
            const VkRect2D&                  renderArea,
            const std::vector<VkClearValue>& clearColors)
        {
            VkRenderPassBeginInfo renderPassBeginInfo = {};
            renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassBeginInfo.renderPass = renderPass;
            renderPassBeginInfo.framebuffer = framebuffer;
            renderPassBeginInfo.renderArea = renderArea;
            renderPassBeginInfo.clearValueCount = static_cast<UINT>(clearColors.size());
            renderPassBeginInfo.pClearValues = clearColors.data();

            VkSubpassContents subpassContents = (m_cmdBufferType == BX_DIRECT_COMMAND_BUFFER) ?
                VK_SUBPASS_CONTENTS_INLINE :
                VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS;

            vkCmdBeginRenderPass(m_cmdBuffer, &renderPassBeginInfo, subpassContents);

            m_cmdStageFlags.render = 1;
        }

        void CmdBuffer::cmdBindVertexBuffers(
            const std::vector<VkBuffer>&     vertexBuffers,
            const std::vector<VkDeviceSize>& offsets)
        {
            assert(m_cmdStageFlags.begin == 1 && m_cmdStageFlags.render == 1);

            vkCmdBindVertexBuffers(m_cmdBuffer,
                                   0,
                                   static_cast<UINT>(vertexBuffers.size()),
                                   vertexBuffers.data(),
                                   offsets.data());
        }

        void CmdBuffer::cmdBindIndexBuffers(
            const VkBuffer&     indexBuffer,
            const VkDeviceSize& offset,
            const BX_INDEX_TYPE indexType)
        {
            assert(m_cmdStageFlags.begin == 1 && m_cmdStageFlags.render == 1);

            vkCmdBindIndexBuffer(m_cmdBuffer,
                                 indexBuffer,
                                 offset,
                                 Utility::VulkanUtility::GetVkIndexType(indexType));
        }

        void CmdBuffer::cmdBindDescriptorSets(
            const VkPipelineLayout&             pipelineLayout,
            const std::vector<VkDescriptorSet>& descriptorSets)
        {
            assert(m_cmdStageFlags.begin == 1 && m_cmdStageFlags.render == 1);

            vkCmdBindDescriptorSets(m_cmdBuffer,
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    pipelineLayout,
                                    0,
                                    static_cast<UINT>(descriptorSets.size()),
                                    descriptorSets.data(),
                                    0,
                                    NULL);
        }

        // TODO: Copy multiple buffers
        void CmdBuffer::cmdCopyBuffer(
            const VkBuffer&         srcBuffer,
            const VkBuffer&         dstBuffer,
            const BxBufferCopyInfo& copyInfo)
        {
            VkBufferCopy bufferCopy = {};
            bufferCopy.size         = copyInfo.copySize;
            bufferCopy.srcOffset    = copyInfo.srcOffset;
            bufferCopy.dstOffset    = copyInfo.dstOffset;

            vkCmdCopyBuffer(m_cmdBuffer, srcBuffer, dstBuffer, 1, &bufferCopy);
        }

        void CmdBuffer::cmdCopyBufferToImage(
            const VkBuffer&                             srcBuffer,
            const VkImage&                              dstImage,
            const std::vector<BxBufferToImageCopyInfo>& copyInfo)
        {
            size_t copyNum = copyInfo.size();

            std::vector<VkBufferImageCopy> bufferImageCopy(copyNum);

            for (size_t i = 0; i < copyNum; ++i)
            {
                bufferImageCopy[i].bufferOffset                    = copyInfo[i].bufferInfo.bufferOffset;
                bufferImageCopy[i].bufferRowLength                 = copyInfo[i].bufferInfo.bufferRowLength;
                bufferImageCopy[i].bufferImageHeight               = copyInfo[i].bufferInfo.bufferImageHeight;
                bufferImageCopy[i].imageOffset                     = copyInfo[i].imageInfo.imageOffset;
                bufferImageCopy[i].imageExtent                     = copyInfo[i].imageInfo.imageExtent;
                bufferImageCopy[i].imageSubresource.aspectMask     = copyInfo[i].subResourceInfo.aspectMask;
                bufferImageCopy[i].imageSubresource.baseArrayLayer = copyInfo[i].subResourceInfo.baseArrayLayer;
                bufferImageCopy[i].imageSubresource.layerCount     = copyInfo[i].subResourceInfo.layerNum;
                bufferImageCopy[i].imageSubresource.mipLevel       = copyInfo[i].subResourceInfo.baseMipLevel;
            }

            vkCmdCopyBufferToImage(m_cmdBuffer,
                                   srcBuffer,
                                   dstImage,
                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                   static_cast<UINT>(copyNum),
                                   bufferImageCopy.data());
        }

        void CmdBuffer::cmdImageLayoutTransition(
            const VkImage&                image,
            const BxLayoutTransitionInfo& layoutTransInfoList)
        {
            size_t layoutTransNum = layoutTransInfoList.subResourceInfo.size();

            std::vector<VkImageMemoryBarrier> imageMemoryBarrierList(layoutTransNum);

            VkImageLayout        oldLayout = layoutTransInfoList.oldLayout;
            VkImageLayout        newLayout = layoutTransInfoList.newLayout;
            VkAccessFlags        srcAccessMask;
            VkAccessFlags        dstAccessMask;
            VkPipelineStageFlags srcStage;
            VkPipelineStageFlags dstStage;

            Utility::VulkanUtility::
                GetImageTransitionAccessMask(oldLayout, newLayout,
                                             &srcAccessMask, &dstAccessMask, &srcStage, &dstStage);

            for (size_t i = 0; i < layoutTransNum; ++i)
            {
                imageMemoryBarrierList[i].sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                imageMemoryBarrierList[i].image                           = image;
                imageMemoryBarrierList[i].oldLayout                       = oldLayout;
                imageMemoryBarrierList[i].newLayout                       = newLayout;
                imageMemoryBarrierList[i].srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
                imageMemoryBarrierList[i].dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
                imageMemoryBarrierList[i].subresourceRange.aspectMask     = layoutTransInfoList.subResourceInfo[i].aspectMask;
                imageMemoryBarrierList[i].subresourceRange.baseArrayLayer = layoutTransInfoList.subResourceInfo[i].baseArrayLayer;
                imageMemoryBarrierList[i].subresourceRange.layerCount     = layoutTransInfoList.subResourceInfo[i].layerNum;
                imageMemoryBarrierList[i].subresourceRange.baseMipLevel   = layoutTransInfoList.subResourceInfo[i].baseMipLevel;
                imageMemoryBarrierList[i].subresourceRange.levelCount     = layoutTransInfoList.subResourceInfo[i].mipmapLevelNum;
                imageMemoryBarrierList[i].srcAccessMask                   = srcAccessMask;
                imageMemoryBarrierList[i].dstAccessMask                   = dstAccessMask;
            }

            vkCmdPipelineBarrier(m_cmdBuffer,
                                 srcStage, dstStage,
                                 0,
                                 0, NULL,
                                 0, NULL,
                                 static_cast<UINT>(imageMemoryBarrierList.size()), imageMemoryBarrierList.data());
        }

        void CmdBuffer::cmdDrawArrays(
            const VkPipeline& graphicsPipeline,
            const UINT        vertexCount,
            const UINT        vertexOffset)
        {
            assert(m_cmdStageFlags.begin == 1 && m_cmdStageFlags.render == 1);

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
            assert(m_cmdStageFlags.begin == 1 && m_cmdStageFlags.render == 1);

            vkCmdBindPipeline(m_cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
            vkCmdDraw(m_cmdBuffer, vertexCount, instanceCount, vertexOffset, instanceOffset);
        }

        void CmdBuffer::cmdDrawElements(
            const VkPipeline& graphicsPipeline,
            const UINT        indexCount,
            const UINT        indexOffset,
            const UINT        vertexOffset)
        {
            vkCmdBindPipeline(m_cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
            vkCmdDrawIndexed(m_cmdBuffer, indexCount, 1, indexOffset, vertexOffset, 0);
        }

        void CmdBuffer::cmdDrawElementsInstanced(
            const VkPipeline& graphicsPipeline,
            const UINT        indexCount,
            const UINT        instanceCount,
            const UINT        indexOffset,
            const UINT        vertexOffset,
            const UINT        instanceOffset)
        {
            vkCmdBindPipeline(m_cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
            vkCmdDrawIndexed(
                m_cmdBuffer, indexCount, instanceCount, indexOffset, vertexOffset, instanceOffset);
        }
    }
}
