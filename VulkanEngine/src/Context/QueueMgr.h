#pragma once

#include "../Core/VulkanPCH.h"

struct QueueFamilyIndices
{
    INT presentSurfaceFamilyIndex = -1;
    INT graphicsFamilyIndex       = -1;
    INT computeFamilyIndex        = -1;

    inline BOOL IsCompleted() const
    {
        return ((graphicsFamilyIndex >= 0) &&
            (computeFamilyIndex >= 0) &&
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
        case 0:
            result = presentSurfaceFamilyIndex;
            break;
        case 1:
            result = graphicsFamilyIndex;
            break;
        case 2:
            result = computeFamilyIndex;
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
    VkQueue m_queue  = VK_NULL_HANDLE;
    float   priority = 1.0f;
};

class QueueMgr
{
public:
    QueueMgr();
    ~QueueMgr();

    QueueFamilyIndices retriveHwQueueIndices(
        const VkPhysicalDevice& hwGpuDevice,
        const VkSurfaceKHR&     surface);

    void retriveQueueHandle(
        const VkDevice& device);

    inline QueueFamilyIndices GetHwQueueIndices() const
    {
        return m_hwQueueIndices;
    }

    inline BOOL IsQueueIndicesCompleted() const
    {
        return m_hwQueueIndices.IsCompleted();
    }

    inline BxQueue GetQueue(const UINT i) const
    {
        BxQueue result;
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
    QueueFamilyIndices m_hwQueueIndices;

    BxQueue m_presentQueue;
    BxQueue m_graphicsQueue;
    BxQueue m_computeQueue;
};