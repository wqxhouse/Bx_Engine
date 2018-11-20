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
#include "../Core/VulkanUtility.h"

namespace VulkanEngine
{
    namespace Buffer
    {
        struct BxBufferCreateInfo
        {
            VkBufferUsageFlags bufferUsage;
            UINT64             bufferSize;
            void*              bufferData;
        };

        class VulkanBufferBase
        {
        public:
            explicit VulkanBufferBase(
                const VkDevice* const pDevice);

            ~VulkanBufferBase();

            BOOL createBuffer(
                const VkPhysicalDevice&   hwDevice,
                const BxBufferCreateInfo& bufferCreateInfo);
            
            BOOL updateBufferData(
                const UINT64      bufferSize,
                const void* const bufferData);

            inline const VkBuffer GetBuffer() const
            {
                return m_vulkanBuffer;
            }

        private:
            const VkDevice* const    m_pDevice;

            VDeleter<VkBuffer>       m_vulkanBuffer;
            VDeleter<VkDeviceMemory> m_vulkanBufferMemory;
        };
    }
}