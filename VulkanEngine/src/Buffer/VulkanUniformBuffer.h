//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "VulkanDescriptorBuffer.h"

namespace VulkanEngine
{
    namespace Buffer
    {
        class VulkanUniformBuffer : public VulkanDescriptorBuffer
        {
        public:
            VulkanUniformBuffer(
                const VkDevice* const pDevice);

            virtual ~VulkanUniformBuffer();

            virtual BOOL createDescriptorSetLayout(
                const UINT               bindingPoint,
                const UINT               descriptorNum,
                const VkShaderStageFlags stageFlags);

            virtual BOOL createUniformBuffer(
                const VkPhysicalDevice& hwDevice,
                const VkDeviceSize      uboNum,
                const VkDeviceSize      uboSize,
                const void*             uboData,
                const BOOL              isdynamic = FALSE);

            INLINE void updateUniformBufferData(
                const void* uboData)
            {
                updateBufferData(m_bufferSize, uboData);
            }
        };
    }
}