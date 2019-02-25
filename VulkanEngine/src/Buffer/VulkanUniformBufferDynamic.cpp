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
            const VkDevice*    const pDevice,
            const VkDeviceSize       minUniformBufferOffsetAlignment)
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

            m_uniformStructNum  = uboNum;
            m_uniformStructSize = calDynamicUniformBufferAlignmentSize(uboSize);

            VkDeviceSize m_dynamicUniformBufferSize = m_uniformStructSize * m_uniformStructNum;

            assert(
                (uboNum                     <= MAX_DYNAMIC_UNIFORM_STRUCT_NUM)  &&
                (m_uniformStructSize        <= MAX_DYNAMIC_UNIFORM_STRUCT_SIZE) &&
                (m_dynamicUniformBufferSize <= MAX_DYNAMIC_UNIFORM_SIZE));

            result = VulkanUniformBuffer::createUniformBuffer(hwDevice,
                                                              uboNum,
                                                              m_uniformStructSize,
                                                              uboData,
                                                              TRUE);

            assert(result == BX_SUCCESS);

            return result;
        }

        BOOL VulkanUniformBufferDynamic::updateUniformBufferDataRange(
            const std::vector<VulkanBufferRangeData>& bufferRangeData)
        {
            BOOL result = BX_SUCCESS;

            assert((IsBufferDynamic()   == TRUE) &&
                   (IsBufferOptimized() == FALSE));

            result = updateBufferDataRange(GetBufferSize(), m_uniformStructSize, bufferRangeData);

            assert(result == FALSE);

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