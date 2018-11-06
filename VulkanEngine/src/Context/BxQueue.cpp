#include "BxQueue.h"

QueueMgr::QueueMgr()
{
}

QueueMgr::~QueueMgr()
{
}

QueueFamilyIndices QueueMgr::retriveHwQueueIndices(
    const VkPhysicalDevice& hwGpuDevice,
    const VkSurfaceKHR&     surface)
{
    UINT queueFamilyNum = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(hwGpuDevice, &queueFamilyNum, NULL);

    if (queueFamilyNum > 0)
    {
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyNum);
        vkGetPhysicalDeviceQueueFamilyProperties(hwGpuDevice, &queueFamilyNum, queueFamilyProperties.data());

        UINT index = 0;
        for (const VkQueueFamilyProperties& prop : queueFamilyProperties)
        {
            if (prop.queueCount > 0)
            {
                if ((prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
                {
                    m_hwQueueIndices.graphicsFamilyIndex = index;
                    if (m_hwQueueIndices.IsCompleted() == TRUE)
                    {
                        //break;
                    }
                }
                else if ((prop.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)
                {
                    m_hwQueueIndices.computeFamilyIndex = index;
                    if (m_hwQueueIndices.IsCompleted() == TRUE)
                    {
                        //break;
                    }
                }

                VkBool32 isSurfacePresentSupport = VK_FALSE;
                vkGetPhysicalDeviceSurfaceSupportKHR(hwGpuDevice, index, surface, &isSurfacePresentSupport);

                if (isSurfacePresentSupport == VK_TRUE)
                {
                    m_hwQueueIndices.presentSurfaceFamilyIndex = index;
                }

                if (m_hwQueueIndices.IsCompleted() == TRUE)
                {
                    break;
                }
            }

            index++;
        }
    }

    return m_hwQueueIndices;
}

void QueueMgr::retriveQueueHandle(
    const VkDevice& device)
{
    vkGetDeviceQueue(device, m_hwQueueIndices.presentSurfaceFamilyIndex, 0, &(m_presentQueue.m_queue));
    vkGetDeviceQueue(device, m_hwQueueIndices.graphicsFamilyIndex,       0, &(m_graphicsQueue.m_queue));
    vkGetDeviceQueue(device, m_hwQueueIndices.computeFamilyIndex,        0, &(m_computeQueue.m_queue));
}
