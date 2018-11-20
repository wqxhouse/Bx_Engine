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
            const VkDevice* const pDevice)
            : m_pDevice(pDevice)
        {
            m_vulkanBuffer       = { *m_pDevice, vkDestroyBuffer };
            m_vulkanBufferMemory = { *m_pDevice, vkFreeMemory    };
        }

        VulkanBufferBase::~VulkanBufferBase()
        {
        }

        BOOL VulkanBufferBase::createBuffer(
            const VkPhysicalDevice&   hwDevice,
            const BxBufferCreateInfo& bufferCreateInfo)
        {
            BOOL result = BX_SUCCESS;

            VkBufferCreateInfo vulkanBufferCreateInfo = {};
            vulkanBufferCreateInfo.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            vulkanBufferCreateInfo.size               = bufferCreateInfo.bufferSize;
            vulkanBufferCreateInfo.usage              = bufferCreateInfo.bufferUsage;
            vulkanBufferCreateInfo.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;

            VkResult vertexBufferCreateResult =
                vkCreateBuffer(*m_pDevice, &vulkanBufferCreateInfo, NULL, m_vulkanBuffer.replace());

            result = Utility::VulkanUtility::GetBxStatus(vertexBufferCreateResult);

            assert(result == BX_SUCCESS);
            if (result == BX_SUCCESS)
            {
                VkMemoryRequirements memRequirements;

                vkGetBufferMemoryRequirements(*m_pDevice, m_vulkanBuffer, &memRequirements);

                VkPhysicalDeviceMemoryProperties hwMemoryProps;
                vkGetPhysicalDeviceMemoryProperties(hwDevice, &hwMemoryProps);

                UINT memTypeIndex = Utility::VulkanUtility::FindMemoryType(
                    hwMemoryProps,
                    memRequirements.memoryTypeBits,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

                assert(memTypeIndex != 0);

                VkMemoryAllocateInfo memAllocInfo = {};
                memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                memAllocInfo.allocationSize = memRequirements.size;
                memAllocInfo.memoryTypeIndex = memTypeIndex;

                VkResult memAllocResult =
                    vkAllocateMemory(*m_pDevice, &memAllocInfo, NULL, m_vulkanBufferMemory.replace());
                result = Utility::VulkanUtility::GetBxStatus(vertexBufferCreateResult);

                assert(result == BX_SUCCESS);

                vkBindBufferMemory(*m_pDevice, m_vulkanBuffer, m_vulkanBufferMemory, 0);

                if (result == BX_SUCCESS)
                {
                    result =
                        updateBufferData(vulkanBufferCreateInfo.size, bufferCreateInfo.bufferData);

                    assert(result == BX_SUCCESS);
                }
            }

            return result;
        }

        BOOL VulkanBufferBase::updateBufferData(
            const UINT64       bufferSize,
            const void * const bufferData)
        {
            BOOL result = BX_SUCCESS;

            void* bufferDataPtr;
            VkResult mapMemoryResult =
                vkMapMemory(*m_pDevice, m_vulkanBufferMemory, 0, bufferSize, 0, &bufferDataPtr);
            result = Utility::VulkanUtility::GetBxStatus(mapMemoryResult);

            assert(result == BX_SUCCESS);

            if (result == BX_SUCCESS)
            {
                memcpy(bufferDataPtr, bufferData, bufferSize);
                vkUnmapMemory(*m_pDevice, m_vulkanBufferMemory);
            }

            return result;
        }
    }
}