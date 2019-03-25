//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2019
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include <Memory/BxMemory.h>

#include "VulkanUniformBuffer.h"

#define MAX_DYNAMIC_UNIFORM_STRUCT_SIZE 256
#define MAX_DYNAMIC_UNIFORM_STRUCT_NUM  512

#define MAX_DYNAMIC_UNIFORM_SIZE ((MAX_DYNAMIC_UNIFORM_STRUCT_NUM) * (MAX_DYNAMIC_UNIFORM_STRUCT_SIZE))

namespace VulkanEngine
{
    namespace Buffer
    {
        class VulkanUniformBufferDynamic : public VulkanUniformBuffer
        {
        public:
            VulkanUniformBufferDynamic(
                const VkDevice*    const pDevice,
                const VkDeviceSize       minUniformBufferOffsetAlignment);

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
            
            BOOL updateUniformBufferDataRange(
                const std::vector<VulkanBufferRangeData>& bufferRangeData);

        private:
            VkDeviceSize calDynamicUniformBufferAlignmentSize(
                const VkDeviceSize uboSize);

            const VkDeviceSize m_minUniformBufferOffsetAlignment;

            VkDeviceSize m_uniformStructSize;
            VkDeviceSize m_uniformStructNum;
        };
    }
}