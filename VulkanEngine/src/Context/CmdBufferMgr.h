#pragma once

#include "../Core/VulkanUtility.h"
#include "../Buffer/CmdBuffer.h"

class CmdBufferMgr
{
public:
    CmdBufferMgr(
        const VkDevice& device,
        const UINT      queueFamilyNum);

    ~CmdBufferMgr();

    BOOL createCmdBufferPool(
        const BX_QUEUE_TYPE queueType,
        const UINT          queueFamilyIndex);

private:
    const VkDevice& m_device;

    std::vector<VDeleter<VkCommandPool>> m_cmdPool;
};