//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2019
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "VulkanUniformBufferDynamic.h"

namespace VulkanEngine
{
    namespace Buffer
    {
        VulkanUniformBufferDynamic::VulkanUniformBufferDynamic(
            const VkDevice* const pDevice,
            const VkDeviceSize    minUniformBufferOffsetAlignment)
            : VulkanUniformBuffer(pDevice),
              m_minUniformBufferOffsetAlignment(minUniformBufferOffsetAlignment)
        {
        }

        VulkanUniformBufferDynamic::~VulkanUniformBufferDynamic()
        {
        }

        BOOL VulkanUniformBufferDynamic::createDescriptorSetLayout(
            const UINT               bindingPoint,
            const UINT               descriptorNum,
            const VkShaderStageFlags stageFlags)
        {
            BOOL result = BX_SUCCESS;

            VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
            descriptorSetLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            descriptorSetLayoutBinding.binding            = bindingPoint;
            descriptorSetLayoutBinding.descriptorCount    = descriptorNum;
            descriptorSetLayoutBinding.stageFlags         = stageFlags;
            descriptorSetLayoutBinding.pImmutableSamplers = NULL;

            VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
            descriptorSetLayoutCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            descriptorSetLayoutCreateInfo.bindingCount = 1;
            descriptorSetLayoutCreateInfo.pBindings    = &descriptorSetLayoutBinding;

            const VkDevice* const pDevice      = GetDevice();
            VkResult descriptorSetCreateResult =
                vkCreateDescriptorSetLayout(*pDevice,
                                            &descriptorSetLayoutCreateInfo,
                                            NULL,
                                            m_descriptorSetLayout.replace());
            result = Utility::VulkanUtility::GetBxStatus(descriptorSetCreateResult);

            return result;
        }

        BOOL VulkanUniformBufferDynamic::createUniformBuffer(
            const VkPhysicalDevice& hwDevice,
            const VkDeviceSize      uboNum,
            const VkDeviceSize      uboSize,
            const void*             uboData)
        {
            BOOL result = BX_SUCCESS;

            VkDeviceSize m_dynamicUniformBufferAlignmentSize = calDynamicUniformBufferAlignmentSize(uboSize);

            result = VulkanUniformBuffer::createUniformBuffer(hwDevice,
                                                              uboNum,
                                                              m_dynamicUniformBufferAlignmentSize,
                                                              uboData);

            assert(result == BX_SUCCESS);

            return result;
        }

        VkDeviceSize VulkanUniformBufferDynamic::calDynamicUniformBufferAlignmentSize(
            const VkDeviceSize uboSize)
        {
            VkDeviceSize alignmentSize;

            assert(m_minUniformBufferOffsetAlignment > 0 &&
                   Math::BitUtils::IsPowOfTwo(m_minUniformBufferOffsetAlignment) == TRUE);

            alignmentSize = ((uboSize + m_minUniformBufferOffsetAlignment - 1) & (~(uboSize - 1)));

            return alignmentSize;
        }
    }
}