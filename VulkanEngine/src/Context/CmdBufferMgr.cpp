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
