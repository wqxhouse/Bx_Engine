//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "VulkanBuffer.h"

namespace VulkanEngine
{
    namespace Buffer
    {
        VulkanBufferBase::VulkanBufferBase(
            const VkDevice* const pDevice,
            Mgr::CmdBufferMgr* const pCmdBufferMgr)
            : m_pDevice(pDevice),
              m_pCmdBufferMgr(pCmdBufferMgr)
        {
            m_hostBuffer       = { *m_pDevice, vkDestroyBuffer };
            m_hostBufferMemory = { *m_pDevice, vkFreeMemory    };

            m_gpuBuffer        = { *m_pDevice, vkDestroyBuffer };
            m_gpuBufferMemory  = { *m_pDevice, vkFreeMemory    };
        }

        VulkanBufferBase::~VulkanBufferBase()
        {
        }

        BOOL VulkanBufferBase::createBuffer(
            const VkPhysicalDevice&   hwDevice,
            const BxBufferCreateInfo& bufferCreateInfo)
        {
            BOOL result = BX_SUCCESS;

            m_enableOptimization = bufferCreateInfo.bufferOptimization;

            if (m_enableOptimization == TRUE)
            {
                BxRawBufferCreateInfo hostBufferCreateInfo = {};
                hostBufferCreateInfo.pBuffer               = m_hostBuffer.replace();
                hostBufferCreateInfo.pBufferMemory         = m_hostBufferMemory.replace();
                hostBufferCreateInfo.bufferUsage           = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                hostBufferCreateInfo.bufferSize            = bufferCreateInfo.bufferSize;
                hostBufferCreateInfo.bufferProperties      = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

                result = createRawBuffer(hwDevice, hostBufferCreateInfo);

                assert(result == BX_SUCCESS);

                if (result == BX_SUCCESS)
                {
                    result = updateHostBufferData(
                        bufferCreateInfo.bufferSize, bufferCreateInfo.bufferData);

                    assert(result == BX_SUCCESS);
                }

                if (result == BX_SUCCESS)
                {
                    BxRawBufferCreateInfo gpuBufferCreateInfo = {};
                    gpuBufferCreateInfo.pBuffer               = m_gpuBuffer.replace();
                    gpuBufferCreateInfo.pBufferMemory         = m_gpuBufferMemory.replace();
                    gpuBufferCreateInfo.bufferUsage           = bufferCreateInfo.bufferUsage |
                                                                VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                    gpuBufferCreateInfo.bufferSize            = hostBufferCreateInfo.bufferSize;
                    gpuBufferCreateInfo.bufferProperties      = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

                    result = createRawBuffer(hwDevice, gpuBufferCreateInfo);

                    assert(result == BX_SUCCESS);
                }

                if (result == BX_SUCCESS)
                {
                    BxBufferCopyInfo bufferCopyInfo = {};
                    bufferCopyInfo.copySize         = bufferCreateInfo.bufferSize;
                    result = copyBuffer(m_hostBuffer, m_gpuBuffer, bufferCopyInfo);

                    assert(result == BX_SUCCESS);

                    m_hostBuffer.~VDeleter();
                    m_hostBufferMemory.~VDeleter();
                }
            }
            else
            {
                BxRawBufferCreateInfo hostBufferCreateInfo = {};
                hostBufferCreateInfo.pBuffer               = m_hostBuffer.replace();
                hostBufferCreateInfo.pBufferMemory         = m_hostBufferMemory.replace();
                hostBufferCreateInfo.bufferUsage           = bufferCreateInfo.bufferUsage;
                hostBufferCreateInfo.bufferSize            = bufferCreateInfo.bufferSize;
                hostBufferCreateInfo.bufferProperties      = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

                result = createRawBuffer(hwDevice, hostBufferCreateInfo);

                assert(result == BX_SUCCESS);

                if (result == BX_SUCCESS)
                {
                    result = updateHostBufferData(
                        bufferCreateInfo.bufferSize, bufferCreateInfo.bufferData);

                    assert(result == BX_SUCCESS);
                }
            }

            return result;
        }

        BOOL VulkanBufferBase::copyBuffer(
            const VkBuffer&          srcBuffer,
            const VkBuffer&          dstBuffer,
            const BxBufferCopyInfo&  copyInfo)
        {
            BOOL result = BX_SUCCESS;

            result = m_pCmdBufferMgr->copyBuffer(srcBuffer, dstBuffer, copyInfo);

            assert(result == BX_SUCCESS);

            return result;
        }

        BOOL VulkanBufferBase::updateHostBufferData(
            const VkDeviceSize bufferSize,
            const void* const  bufferData)
        {
            BOOL result = BX_SUCCESS;

            void* bufferDataPtr;
            VkResult mapMemoryResult =
                vkMapMemory(*m_pDevice, m_hostBufferMemory, 0, bufferSize, 0, &bufferDataPtr);
            result = Utility::VulkanUtility::GetBxStatus(mapMemoryResult);

            assert(result == BX_SUCCESS);

            if (result == BX_SUCCESS)
            {
                memcpy(bufferDataPtr, bufferData, bufferSize);
                vkUnmapMemory(*m_pDevice, m_hostBufferMemory);
            }

            return result;
        }

        BOOL VulkanBufferBase::createRawBuffer(
            const VkPhysicalDevice&      hwDevice,
            const BxRawBufferCreateInfo& rawBufferCreateInfo)
        {
            BOOL result = BX_SUCCESS;

            VkBufferCreateInfo vulkanBufferCreateInfo = {};
            vulkanBufferCreateInfo.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            vulkanBufferCreateInfo.size               = rawBufferCreateInfo.bufferSize;
            vulkanBufferCreateInfo.usage              = rawBufferCreateInfo.bufferUsage;
            vulkanBufferCreateInfo.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;

            VkBuffer*       const pBuffer       = rawBufferCreateInfo.pBuffer;
            VkDeviceMemory* const pBufferMemory = rawBufferCreateInfo.pBufferMemory;

            VkResult vertexBufferCreateResult =
                vkCreateBuffer(*m_pDevice, &vulkanBufferCreateInfo, NULL, pBuffer);

            result = Utility::VulkanUtility::GetBxStatus(vertexBufferCreateResult);

            assert(result == BX_SUCCESS);
            if (result == BX_SUCCESS)
            {
                VkMemoryRequirements memRequirements;

                vkGetBufferMemoryRequirements(*m_pDevice, *pBuffer, &memRequirements);

                VkPhysicalDeviceMemoryProperties hwMemoryProps;
                vkGetPhysicalDeviceMemoryProperties(hwDevice, &hwMemoryProps);

                UINT memTypeIndex = Utility::VulkanUtility::FindMemoryType(
                    hwMemoryProps,
                    memRequirements.memoryTypeBits,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

                assert(memTypeIndex != 0);

                VkMemoryAllocateInfo memAllocInfo = {};
                memAllocInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                memAllocInfo.allocationSize       = memRequirements.size;
                memAllocInfo.memoryTypeIndex      = memTypeIndex;

                VkResult memAllocResult =
                    vkAllocateMemory(*m_pDevice, &memAllocInfo, NULL, pBufferMemory);
                result = Utility::VulkanUtility::GetBxStatus(vertexBufferCreateResult);

                assert(result == BX_SUCCESS);

                vkBindBufferMemory(*m_pDevice, *pBuffer, *pBufferMemory, 0);
            }

            return result;
        }
    }
}