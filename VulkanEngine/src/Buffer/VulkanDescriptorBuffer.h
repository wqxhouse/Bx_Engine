//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "VulkanBuffer.h"

namespace VulkanEngine
{
    namespace Buffer
    {
        class VulkanDescriptorBuffer : public VulkanBufferBase
        {
        public:
            VulkanDescriptorBuffer(
                const VkDevice*    const pDevice,
                Mgr::CmdBufferMgr* const pCmdBufferMgr);

            virtual ~VulkanDescriptorBuffer();

            virtual BOOL createDescriptorSetLayout(
                const UINT               bindingPoint,
                const UINT               descriptorNum,
                const VkShaderStageFlags stageFlags) = 0;

            template <typename T>
            void GetBufferData(T** ppData) const
            {
                const VkDevice       device = *GetDevice();
                const VkDeviceMemory memory = ((IsBufferOptimized() == TRUE) ? m_gpuBufferMemory : m_hostBufferMemory);

                vkMapMemory(device, memory, 0, m_bufferSize, 0, reinterpret_cast<void**>(ppData));
                vkUnmapMemory(device, memory);
            }

            INLINE const VkDescriptorSetLayout GetDescriptorSetLayout() const
            {
                return m_descriptorSetLayout;
            }

            INLINE const VkDeviceSize GetBufferSize()           const { return m_bufferSize;           }
            INLINE const VkDeviceSize GetDescriptorObjectSize() const { return m_descriptorObjectSize; }

        protected:
            VDeleter<VkDescriptorSetLayout> m_descriptorSetLayout;

            VkDeviceSize m_bufferSize;
            VkDeviceSize m_descriptorObjectSize;
        };
    }
}