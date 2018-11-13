#include "CmdBufferMgr.h"

CmdBufferMgr::CmdBufferMgr(
    const VkDevice& device,
    const UINT      queueFamilyNum)
    : m_device(device)
{
    m_cmdPool.resize(queueFamilyNum, { m_device, vkDestroyCommandPool });
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
    cmdPoolCreateInfo.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolCreateInfo.queueFamilyIndex        = queueFamilyIndex;
    cmdPoolCreateInfo.flags                   = 0;

    VkResult cmdPoolCreateResult =
        vkCreateCommandPool(m_device, &cmdPoolCreateInfo, NULL, m_cmdPool[queueType].replace());

    status = VulkanUtility::GetBxStatus(cmdPoolCreateResult);

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
    cmdBufferAllocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufferAllocInfo.commandPool        = m_cmdPool[queueType];
    cmdBufferAllocInfo.level              = VulkanUtility::GetVkCmdBufferLevel(level);
    cmdBufferAllocInfo.commandBufferCount = size;

    std::vector<VkCommandBuffer> graphicsCmdBuffers(size);
    VkResult cmdBufferAllocResult =
        vkAllocateCommandBuffers(m_device, &cmdBufferAllocInfo, graphicsCmdBuffers.data());

    status = VulkanUtility::GetBxStatus(cmdBufferAllocResult);

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
