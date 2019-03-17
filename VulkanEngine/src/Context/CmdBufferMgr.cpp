//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "CmdBufferMgr.h"

namespace VulkanEngine
{
    namespace Mgr
    {
        CmdBufferMgr::CmdBufferMgr(
            const VkDevice* const pDevice,
            const QueueMgr* const pQueueMgr,
            const UINT            queueFamilyNum)
            : m_pDevice(pDevice),
              m_pQueueMgr(pQueueMgr)
        {
            m_cmdPool.resize(queueFamilyNum, { *m_pDevice, vkDestroyCommandPool });
        }

        CmdBufferMgr::~CmdBufferMgr()
        {
        }

        BOOL CmdBufferMgr::createCmdBufferPool(
            const BX_QUEUE_TYPE queueType,
            const UINT          queueFamilyIndex)
        {
            BOOL status = BX_SUCCESS;

            VkCommandPoolCreateInfo cmdPoolCreateInfo = {};
            cmdPoolCreateInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            cmdPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;
            cmdPoolCreateInfo.flags            = 0;

            VkResult cmdPoolCreateResult =
                vkCreateCommandPool(*m_pDevice, &cmdPoolCreateInfo, NULL, m_cmdPool[queueType].replace());

            status = Utility::VulkanUtility::GetBxStatus(cmdPoolCreateResult);

            assert(status == BX_SUCCESS);

            return status;
        }

        BOOL CmdBufferMgr::addGraphicsCmdBuffers(
            const BX_QUEUE_TYPE           queueType,
            const BX_COMMAND_BUFFER_LEVLE level,
            const UINT                    size)
        {
            BOOL status = BX_SUCCESS;

            VkCommandBufferAllocateInfo cmdBufferAllocInfo = {};
            cmdBufferAllocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmdBufferAllocInfo.commandPool                 = m_cmdPool[queueType];
            cmdBufferAllocInfo.level                       = Utility::VulkanUtility::GetVkCmdBufferLevel(level);
            cmdBufferAllocInfo.commandBufferCount          = size;

            std::vector<VkCommandBuffer> graphicsCmdBuffers(size);
            VkResult cmdBufferAllocResult =
                vkAllocateCommandBuffers(*m_pDevice, &cmdBufferAllocInfo, graphicsCmdBuffers.data());
            status = Utility::VulkanUtility::GetBxStatus(cmdBufferAllocResult);

            assert(status == BX_SUCCESS);

            if (status == BX_SUCCESS)
            {
                for (size_t i = 0; i < size; ++i)
                {
                    m_graphicsCmdBuffers.push_back(
                        { BX_GRAPHICS_COMMAND_BUFFER, level, graphicsCmdBuffers[i] });
                }
            }

            return status;
        }

        BOOL CmdBufferMgr::copyBuffer(
            const VkBuffer&                 srcBuffer,
            const VkBuffer&                 dstBuffer,
            const Buffer::BxBufferCopyInfo& copyInfo)
        {
            BOOL status = BX_SUCCESS;

            Buffer::BxCmdBufferCreateInfo cmdBufferCreateInfo = {};
            cmdBufferCreateInfo.cmdBufferType                 = BX_GRAPHICS_COMMAND_BUFFER;
            cmdBufferCreateInfo.bufferLevel                   = BX_DIRECT_COMMAND_BUFFER;
            cmdBufferCreateInfo.pCommandPool                  = &(m_cmdPool[BX_QUEUE_GRAPHICS]);

            Buffer::CmdBuffer copyCommandBuffer =
                Buffer::CmdBuffer::CreateCmdBuffer(m_pDevice, cmdBufferCreateInfo);
            status = copyCommandBuffer.beginCmdBuffer(FALSE);

            assert (status == BX_SUCCESS);

            if (status == BX_SUCCESS)
            {
                copyCommandBuffer.cmdCopyBuffer(srcBuffer, dstBuffer, copyInfo);
                status = copyCommandBuffer.endCmdBuffer();

                assert(status == BX_SUCCESS);
            }

            VkSubmitInfo copyCommandSubmitInfo       = {};
            copyCommandSubmitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            copyCommandSubmitInfo.commandBufferCount = 1;
            copyCommandSubmitInfo.pCommandBuffers    = copyCommandBuffer.GetCmdBufferPtr();

            status = submitCommandBufferToQueue(
                m_pQueueMgr->GetHwQueueIndices().graphicsFamilyIndex, 1, &copyCommandSubmitInfo, VK_NULL_HANDLE);

            assert(status == BX_SUCCESS);

            freeCommandBuffer(m_cmdPool[BX_QUEUE_GRAPHICS], 1, &copyCommandBuffer);

            return status;
        }

        BOOL CmdBufferMgr::copyBufferToImage(
            const VkBuffer&                                      srcBuffer,
            const VkImage&                                       dstImage,
            const std::vector< Buffer::BxBufferToImageCopyInfo>& copyInfo)
        {
            BOOL status = BX_SUCCESS;

            Buffer::BxCmdBufferCreateInfo cmdBufferCreateInfo = {};
            cmdBufferCreateInfo.cmdBufferType                 = BX_GRAPHICS_COMMAND_BUFFER;
            cmdBufferCreateInfo.bufferLevel                   = BX_DIRECT_COMMAND_BUFFER;
            cmdBufferCreateInfo.pCommandPool                  = &(m_cmdPool[BX_QUEUE_GRAPHICS]);

            Buffer::CmdBuffer copyCommandBuffer =
                Buffer::CmdBuffer::CreateCmdBuffer(m_pDevice, cmdBufferCreateInfo);
            status = copyCommandBuffer.beginCmdBuffer(FALSE);

            assert(status == BX_SUCCESS);

            if (status == BX_SUCCESS)
            {
                copyCommandBuffer.cmdCopyBufferToImage(srcBuffer, dstImage, copyInfo);
                status = copyCommandBuffer.endCmdBuffer();

                assert(status == BX_SUCCESS);
            }

            VkSubmitInfo copyCommandSubmitInfo       = {};
            copyCommandSubmitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            copyCommandSubmitInfo.commandBufferCount = 1;
            copyCommandSubmitInfo.pCommandBuffers    = copyCommandBuffer.GetCmdBufferPtr();

            status = submitCommandBufferToQueue(
                m_pQueueMgr->GetHwQueueIndices().graphicsFamilyIndex, 1, &copyCommandSubmitInfo, VK_NULL_HANDLE);

            assert(status == BX_SUCCESS);

            freeCommandBuffer(m_cmdPool[BX_QUEUE_GRAPHICS], 1, &copyCommandBuffer);

            return status;
        }

        BOOL CmdBufferMgr::imageLayoutTransition(
            const VkImage&                        image,
            const Buffer::BxLayoutTransitionInfo& layoutTransInfoList)
        {
            BOOL status = BX_SUCCESS;

            Buffer::BxCmdBufferCreateInfo cmdBufferCreateInfo = {};
            cmdBufferCreateInfo.cmdBufferType                 = BX_GRAPHICS_COMMAND_BUFFER;
            cmdBufferCreateInfo.bufferLevel                   = BX_DIRECT_COMMAND_BUFFER;
            cmdBufferCreateInfo.pCommandPool                  = &(m_cmdPool[BX_QUEUE_GRAPHICS]);

            Buffer::CmdBuffer imageLayoutTransitionBuffer =
                Buffer::CmdBuffer::CreateCmdBuffer(m_pDevice, cmdBufferCreateInfo);
            status = imageLayoutTransitionBuffer.beginCmdBuffer(FALSE);

            assert(status == BX_SUCCESS);

            if (status == BX_SUCCESS)
            {
                imageLayoutTransitionBuffer.cmdImageLayoutTransition(image, layoutTransInfoList);
                status = imageLayoutTransitionBuffer.endCmdBuffer();

                assert(status == BX_SUCCESS);
            }

            VkSubmitInfo copyCommandSubmitInfo       = {};
            copyCommandSubmitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            copyCommandSubmitInfo.commandBufferCount = 1;
            copyCommandSubmitInfo.pCommandBuffers    = imageLayoutTransitionBuffer.GetCmdBufferPtr();

            status = submitCommandBufferToQueue(
                m_pQueueMgr->GetHwQueueIndices().graphicsFamilyIndex, 1, &copyCommandSubmitInfo, VK_NULL_HANDLE);

            assert(status == BX_SUCCESS);

            freeCommandBuffer(m_cmdPool[BX_QUEUE_GRAPHICS], 1, &imageLayoutTransitionBuffer);

            return status;
        }

        BOOL CmdBufferMgr::genMipmaps(
            const VkImage& image,
            const UINT     width,
            const UINT     height,
            const UINT     levels,
            const UINT     layers)
        {
            BOOL status = BX_SUCCESS;

            Buffer::BxCmdBufferCreateInfo cmdBufferCreateInfo = {};
            cmdBufferCreateInfo.cmdBufferType                 = BX_GRAPHICS_COMMAND_BUFFER;
            cmdBufferCreateInfo.bufferLevel                   = BX_DIRECT_COMMAND_BUFFER;
            cmdBufferCreateInfo.pCommandPool                  = &(m_cmdPool[BX_QUEUE_GRAPHICS]);

            Buffer::CmdBuffer imageBlitCmdBuffer =
                Buffer::CmdBuffer::CreateCmdBuffer(m_pDevice, cmdBufferCreateInfo);

            UINT prevMipWidth = width;
            UINT prevMipHeight = height;

            status = imageBlitCmdBuffer.beginCmdBuffer(FALSE);
            assert(status == BX_SUCCESS);

            if (status == BX_SUCCESS)
            {
                for (UINT mipLevel = 1; mipLevel < levels; ++mipLevel)
                {
                    UINT mipWidth  = std::max((prevMipWidth >> 1), 1u);
                    UINT mipHeight = std::max((prevMipHeight >> 1), 1u);

                    // We need to transfer every mipLevel - 1 image to VK_IMAGE_USAGE_TRANSFER_SRC_OPTIMAL before blit
                    Buffer::BxLayoutTransitionInfo transitionInfo = {};
                    transitionInfo.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                    transitionInfo.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                    transitionInfo.subResourceInfo.push_back({ VK_IMAGE_ASPECT_COLOR_BIT, mipLevel - 1, 1, 0, layers });

                    imageBlitCmdBuffer.cmdImageLayoutTransition(image, transitionInfo);

                    // Blit the image to do downsampling
                    imageBlitCmdBuffer.cmdBlitImage(
                        image, image, prevMipWidth, prevMipHeight, mipLevel - 1, mipWidth, mipHeight, mipLevel, layers);

                    prevMipWidth  = mipWidth;
                    prevMipHeight = mipHeight;
                }

                // Transfer the last layer to VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL for unifying
                // all levels' layout in the same image
                Buffer::BxLayoutTransitionInfo postTransitionInfo = {};
                postTransitionInfo.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                postTransitionInfo.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                postTransitionInfo.subResourceInfo.push_back({ VK_IMAGE_ASPECT_COLOR_BIT, levels - 1, 1, 0, layers });
                imageBlitCmdBuffer.cmdImageLayoutTransition(image, postTransitionInfo);
            }

            status = imageBlitCmdBuffer.endCmdBuffer();

            assert(status == BX_SUCCESS);

            VkSubmitInfo genMipmapCommandSubmitInfo       = {};
            genMipmapCommandSubmitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            genMipmapCommandSubmitInfo.commandBufferCount = 1;
            genMipmapCommandSubmitInfo.pCommandBuffers    = imageBlitCmdBuffer.GetCmdBufferPtr();

            status = submitCommandBufferToQueue(
                m_pQueueMgr->GetHwQueueIndices().graphicsFamilyIndex, 1, &genMipmapCommandSubmitInfo, VK_NULL_HANDLE);

            assert(status == BX_SUCCESS);

            freeCommandBuffer(m_cmdPool[BX_QUEUE_GRAPHICS], 1, &imageBlitCmdBuffer);

            return status;
        }

        BOOL CmdBufferMgr::submitCommandBufferToQueue(
            const UINT               queueFamilyIndex,
            const UINT               submitNum,
            const VkSubmitInfo*      submitInfos,
            const VkFence&           fence)
        {
            BOOL status = BX_SUCCESS;

            const VkQueue& submitQueue =
                m_pQueueMgr->GetQueue(queueFamilyIndex).m_queue;

            vkQueueSubmit(submitQueue, submitNum, submitInfos, fence);
            vkQueueWaitIdle(submitQueue);

            return status;
        }

        void CmdBufferMgr::freeCommandBuffer(
            const VkCommandPool&     cmdPool,
            const UINT               bufferNum,
            const Buffer::CmdBuffer* pCmdBuffers)
        {
            std::vector<VkCommandBuffer> cmdBuffers(bufferNum);
            for (UINT i = 0; i < bufferNum; ++i)
            {
                cmdBuffers.push_back(pCmdBuffers->GetCmdBuffer());
            }

            vkFreeCommandBuffers(*m_pDevice, cmdPool, bufferNum, cmdBuffers.data());
        }
    }
}