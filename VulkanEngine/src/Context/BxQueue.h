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

namespace VulkanEngine
{
    namespace Queue
    {
        struct QueueFamilyIndices
        {
            INT graphicsFamilyIndex = -1;
            INT computeFamilyIndex = -1;
            INT presentSurfaceFamilyIndex = -1;

            inline BOOL IsCompleted() const
            {
                return ((graphicsFamilyIndex >= 0) &&
                        //(computeFamilyIndex >= 0) &&
                        (presentSurfaceFamilyIndex >= 0));
            }

            inline UINT GetIndexNum() const
            {
                return (sizeof(QueueFamilyIndices) / sizeof(UINT));
            }

            inline UINT GetQueueFamilyIndex(const UINT i) const
            {
                UINT result;
                switch (i)
                {
                case BX_QUEUE_GRAPHICS:
                    result = graphicsFamilyIndex;
                    break;
                case BX_QUEUE_COMPUTE:
                    result = computeFamilyIndex;
                    break;
                case BX_QUEUE_PRESENT:
                    result = presentSurfaceFamilyIndex;
                    break;
                default:
                    result = -1;
                    break;
                }

                return result;
            }
        };

        struct BxQueue
        {
            VkQueue m_queue = VK_NULL_HANDLE;
            float   priority = 1.0f;
        };
    }

    namespace Mgr
    {
        class QueueMgr
        {
        public:
            QueueMgr();
            ~QueueMgr();

            Queue::QueueFamilyIndices retriveHwQueueIndices(
                const VkPhysicalDevice& hwGpuDevice,
                const VkSurfaceKHR&     surface);

            void retriveQueueHandle(
                const VkDevice& device);

            inline Queue::QueueFamilyIndices GetHwQueueIndices() const
            {
                return m_hwQueueIndices;
            }

            inline BOOL IsQueueIndicesCompleted() const
            {
                return m_hwQueueIndices.IsCompleted();
            }

            inline Queue::BxQueue GetQueue(const UINT i) const
            {
                Queue::BxQueue result;
                switch (i)
                {
                case 0:
                    result = m_presentQueue;
                    break;
                case 1:
                    result = m_graphicsQueue;
                    break;
                case 2:
                    result = m_computeQueue;
                    break;
                default:
                    printf("Query index exceed the queue number!\n");
                    assert(FALSE);
                    break;
                }

                return result;
            }

        private:
            Queue::QueueFamilyIndices m_hwQueueIndices;

            Queue::BxQueue m_presentQueue;
            Queue::BxQueue m_graphicsQueue;
            Queue::BxQueue m_computeQueue;
        };
    }
}