//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
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
        VulkanUniformBuffer::VulkanUniformBuffer(
            const VkDevice* const pDevice)
            : VulkanDescriptorBuffer(pDevice, NULL)
        {
        }

        VulkanUniformBuffer::~VulkanUniformBuffer()
        {
        }

        BOOL VulkanUniformBuffer::createDescriptorSetLayout(
            const UINT               bindingPoint,
            const UINT               descriptorNum,
            const VkShaderStageFlags stageFlags)
        {
            BOOL result = BX_SUCCESS;

            VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
            descriptorSetLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
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

        BOOL VulkanUniformBuffer::createUniformBuffer(
            const VkPhysicalDevice& hwDevice,
            const VkDeviceSize      uboSize,
            const void*             uboData)
        {
            BOOL result = BX_SUCCESS;

            m_bufferSize = uboSize;

            BxBufferCreateInfo uboCreateInfo = {};
            uboCreateInfo.bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            uboCreateInfo.bufferSize  = uboSize;
            uboCreateInfo.bufferData  = uboData;

            result = createBuffer(hwDevice, uboCreateInfo);

            assert(result == BX_SUCCESS);

            return result;
        }
    }
}