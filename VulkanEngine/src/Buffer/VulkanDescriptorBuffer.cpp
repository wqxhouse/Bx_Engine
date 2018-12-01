//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "VulkanDescriptorBuffer.h"

namespace VulkanEngine
{
    namespace Buffer
    {
        VulkanDescriptorBuffer::VulkanDescriptorBuffer(
            const VkDevice*    const    pDevice,
            Mgr::CmdBufferMgr* const    pCmdBufferMgr)
            : VulkanBufferBase(pDevice, pCmdBufferMgr)
        {
            m_descriptorSetLayout = { *pDevice, vkDestroyDescriptorSetLayout };
        }

        VulkanDescriptorBuffer::~VulkanDescriptorBuffer()
        {
        }
    }
}
