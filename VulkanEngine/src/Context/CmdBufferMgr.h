//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../Buffer/CmdBuffer.h"

namespace VulkanEngine
{
    namespace Mgr
    {
        class CmdBufferMgr
        {
        public:
            CmdBufferMgr(
                const VkDevice* const pDevice,
                const QueueMgr* const pQueueMgr,
                const UINT            queueFamilyNum);

            ~CmdBufferMgr();

            BOOL createCmdBufferPool(
                const BX_QUEUE_TYPE queueType,
                const UINT          queueFamilyIndex);

            BOOL addGraphicsCmdBuffers(
                const BX_QUEUE_TYPE           queueType,
                const BX_COMMAND_BUFFER_LEVLE level,
                const UINT                    size,
                const BOOL                    isMainRenderPassCmd);

            BOOL copyBuffer(
                const VkBuffer&                 srcBuffer,
                const VkBuffer&                 dstBuffer,
                const Buffer::BxBufferCopyInfo& copyInfo);

            BOOL copyBufferToImage(
                const VkBuffer&                                      srcBuffer,
                const VkImage&                                       dstImage,
                const std::vector< Buffer::BxBufferToImageCopyInfo>& copyInfo);

            BOOL imageLayoutTransition(
                const VkImage&                        image,
                const Buffer::BxLayoutTransitionInfo& layoutTransInfoList);

            BOOL genMipmaps(
                const VkImage& image,
                const UINT     width,
                const UINT     height,
                const UINT     levels,
                const UINT     layers);

            BOOL submitCommandBufferToQueue(
                const UINT               queueFamilyIndex,
                const UINT               submitNum,
                const VkSubmitInfo*      submitInfos,
                const VkFence&           fence);

            void freeCommandBuffer(
                const VkCommandPool&     cmdPool,
                const UINT               bufferNum,
                const Buffer::CmdBuffer* pCmdBuffers);

            INLINE const UINT GetGraphicsCmdBufferNum() const
            {
                return static_cast<UINT>(m_graphicsCmdBuffers.size());
            }

            INLINE const UINT GetComputeCmdBufferNum() const
            {
                return static_cast<UINT>(m_computeCmdBuffers.size());
            }

            INLINE Buffer::CmdBuffer* GetCmdBuffer(
                const BX_COMMAND_BUFFER_TYPE type,
                const UINT                   index)
            {
                Buffer::CmdBuffer* pCmdBuf = NULL;

                switch (type)
                {
                    case BX_GRAPHICS_COMMAND_BUFFER:
                        pCmdBuf = &(m_graphicsCmdBuffers[index]);
                        break;
                    case BX_COMPUTE_COMMAND_BUFFER:
                        pCmdBuf = &(m_computeCmdBuffers[index]);
                        break;
                    default:
                        printf("Unsupported command buffer type!\n");
                        assert(FALSE);
                        break;
                }

                return pCmdBuf;
            }

        private:
            const VkDevice* m_pDevice;
            const QueueMgr* m_pQueueMgr;

            std::vector<VDeleter<VkCommandPool>> m_cmdPool;

            std::vector<Buffer::CmdBuffer> m_graphicsCmdBuffers;
            std::vector<Buffer::CmdBuffer> m_computeCmdBuffers;
        };
    }
}