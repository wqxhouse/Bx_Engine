//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2019
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "VulkanUniformBuffer.h"

namespace VulkanEngine
{
    namespace Buffer
    {
        class VulkanUniformBufferDynamic : public VulkanUniformBuffer
        {
        public:
            VulkanUniformBufferDynamic(
                const VkDevice* const pDevice,
                const VkDeviceSize    minUniformBufferOffsetAlignment);

            ~VulkanUniformBufferDynamic();

            BOOL createDescriptorSetLayout(
                const UINT               bindingPoint,
                const UINT               descriptorNum,
                const VkShaderStageFlags stageFlags);

            BOOL createUniformBuffer(
                const VkPhysicalDevice& hwDevice,
                const VkDeviceSize      uboNum,
                const VkDeviceSize      uboSize,
                const void*             uboData);
            
        private:
            VkDeviceSize calDynamicUniformBufferAlignmentSize(
                const VkDeviceSize uboSize);

            const VkDeviceSize m_minUniformBufferOffsetAlignment;
        };
    }
}