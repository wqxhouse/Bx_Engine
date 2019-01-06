//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "DescriptorMgr.h"
namespace VulkanEngine
{
    namespace Mgr
    {
        DescriptorMgr::DescriptorMgr(const VkDevice* const pDevice)
            : m_pDevice(pDevice)
        {
            m_descriptorPool = { *pDevice, vkDestroyDescriptorPool };
            m_descriptors.resize(2);
        }

        DescriptorMgr::~DescriptorMgr()
        {
        }

        BOOL DescriptorMgr::createDescriptorPool(
            const std::vector<DescriptorPoolCreateInfo>& descriptorPoolCreateData,
            const UINT                                   descriptorMaxSet)
        {
            BOOL result = BX_SUCCESS;

            size_t descriptorPoolCreateNum = descriptorPoolCreateData.size();

            std::vector<VkDescriptorPoolSize> descriptorPoolSizeList(descriptorPoolCreateNum);

            for (size_t i = 0; i < descriptorPoolCreateNum; ++i)
            {
                descriptorPoolSizeList[i].type =
                    Utility::VulkanUtility::GetVkDescriptorType(descriptorPoolCreateData[i].descriptorType);
                descriptorPoolSizeList[i].descriptorCount = descriptorPoolCreateData[i].descriptorNum;
                
            }

            VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
            descriptorPoolCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            descriptorPoolCreateInfo.poolSizeCount = static_cast<UINT>(descriptorPoolSizeList.size());
            descriptorPoolCreateInfo.pPoolSizes    = descriptorPoolSizeList.data();
            descriptorPoolCreateInfo.maxSets       = descriptorMaxSet;

            VkResult descriptorPoolCreateResult =
                vkCreateDescriptorPool(*m_pDevice,
                                       &descriptorPoolCreateInfo,
                                       NULL,
                                       m_descriptorPool.replace());

            result = Utility::VulkanUtility::GetBxStatus(descriptorPoolCreateResult);

            assert(result == BX_SUCCESS);

            return result;
        }
        
        BOOL DescriptorMgr::createDescriptorSets(
            const BX_DESCRIPTOR_TYPE              descriptorType,
            const Buffer::VulkanDescriptorBuffer* pDescriptorBuffer,
            const UINT                            descriptorSetNum)
        {
            BOOL result = BX_SUCCESS;

            std::vector<VkDescriptorSetLayout> descriptorSetLayoutList(descriptorSetNum);
            const Buffer::VulkanDescriptorBuffer* descriptorBufferIter = pDescriptorBuffer;

            for (UINT i = 0; i < descriptorSetNum; ++i)
            {
                descriptorSetLayoutList[i] = descriptorBufferIter->GetDescriptorSetLayout();
                descriptorBufferIter++;
            }

            m_descriptors[descriptorType].m_descriptorSets.resize(descriptorSetNum);

            VkDescriptorSetAllocateInfo descriptorSetAllocInfo = {};
            descriptorSetAllocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            descriptorSetAllocInfo.descriptorPool     = m_descriptorPool;
            descriptorSetAllocInfo.pSetLayouts        = descriptorSetLayoutList.data();
            descriptorSetAllocInfo.descriptorSetCount = descriptorSetNum;

            VkResult allocateDescriptorSetsResult =
                vkAllocateDescriptorSets(*m_pDevice,
                                         &descriptorSetAllocInfo,
                                         m_descriptors[descriptorType].m_descriptorSets.data());

            result = Utility::VulkanUtility::GetBxStatus(allocateDescriptorSetsResult);

            assert(result == BX_SUCCESS);

            return result;
        }
        
        BOOL DescriptorMgr::updateUniformDescriptorSet(
            const BX_DESCRIPTOR_TYPE              descriptorType,
            const Buffer::VulkanDescriptorBuffer* pDescriptorBuffer,
            const UINT                            descriptorSetIndex)
        {
            BOOL result = BX_SUCCESS;
            
            VkDescriptorBufferInfo descriptorBufferInfo = {};
            descriptorBufferInfo.buffer = pDescriptorBuffer->GetBuffer();
            descriptorBufferInfo.offset = 0;
            descriptorBufferInfo.range  = pDescriptorBuffer->GetBufferSize();

            VkWriteDescriptorSet writeDescriptorSet = {};
            writeDescriptorSet.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSet.descriptorType       =
                Utility::VulkanUtility::GetVkDescriptorType(descriptorType);
            writeDescriptorSet.descriptorCount      = 1;
            writeDescriptorSet.dstSet               = m_descriptors[descriptorType].m_descriptorSets[descriptorSetIndex];
            writeDescriptorSet.dstBinding           = 0;
            writeDescriptorSet.dstArrayElement      = 0;
            writeDescriptorSet.pBufferInfo          = &descriptorBufferInfo;

            vkUpdateDescriptorSets(*m_pDevice, 1, &writeDescriptorSet, 0, NULL);

            return result;
        }
    }
}
