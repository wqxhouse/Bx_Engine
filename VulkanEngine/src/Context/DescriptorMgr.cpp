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
        DescriptorMgr::DescriptorMgr(
            const VkDevice* const pDevice)
            : m_pDevice(pDevice)
        {
            m_descriptorPool      = { *pDevice, vkDestroyDescriptorPool      };
            m_descriptorSetLayout = { *pDevice, vkDestroyDescriptorSetLayout };
            m_descriptors.resize(3);
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

        BOOL DescriptorMgr::createDescriptorSetLayout(
            const std::vector<DescriptorCreateInfo>& descriptorsCreateInfo)
        {
            BOOL result = BX_SUCCESS;

            size_t descriptorSetLayoutNum = descriptorsCreateInfo.size();

            std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindingList(descriptorSetLayoutNum);

            for (size_t i = 0; i < descriptorSetLayoutNum; ++i)
            {
                descriptorSetLayoutBindingList[i].descriptorType     =
                    Utility::VulkanUtility::GetVkDescriptorType(descriptorsCreateInfo[i].descriptorType);
                descriptorSetLayoutBindingList[i].binding            = descriptorsCreateInfo[i].bindingPoint;
                descriptorSetLayoutBindingList[i].descriptorCount    = descriptorsCreateInfo[i].descriptorNum;
                descriptorSetLayoutBindingList[i].stageFlags         =
                    Utility::VulkanUtility::GetVkShaderStageFlag(descriptorsCreateInfo[i].shaderType);
                descriptorSetLayoutBindingList[i].pImmutableSamplers = NULL;
            }

            VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
            descriptorSetLayoutCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            descriptorSetLayoutCreateInfo.bindingCount = static_cast<UINT>(descriptorSetLayoutBindingList.size());
            descriptorSetLayoutCreateInfo.pBindings    = descriptorSetLayoutBindingList.data();

            VkResult descriptorSetCreateResult =
                vkCreateDescriptorSetLayout(*m_pDevice,
                                            &descriptorSetLayoutCreateInfo,
                                            NULL,
                                            m_descriptorSetLayout.replace());

            result = Utility::VulkanUtility::GetBxStatus(descriptorSetCreateResult);

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

            m_descriptors[0].m_descriptorSets.resize(descriptorSetNum);

            VkDescriptorSetAllocateInfo descriptorSetAllocInfo = {};
            descriptorSetAllocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            descriptorSetAllocInfo.descriptorPool     = m_descriptorPool;
            descriptorSetAllocInfo.pSetLayouts        = descriptorSetLayoutList.data();
            descriptorSetAllocInfo.descriptorSetCount = descriptorSetNum;

            VkResult allocateDescriptorSetsResult =
                vkAllocateDescriptorSets(*m_pDevice,
                                         &descriptorSetAllocInfo,
                                         m_descriptors[0].m_descriptorSets.data());

            result = Utility::VulkanUtility::GetBxStatus(allocateDescriptorSetsResult);

            assert(result == BX_SUCCESS);

            return result;
        }
        
        BOOL DescriptorMgr::updateDescriptorSet(
            const std::vector<DescriptorUpdateInfo>& descriptorUpdateData)
        {
            BOOL result = BX_SUCCESS;
            
            size_t descriptorUpdateNum = descriptorUpdateData.size();

            std::vector<VkWriteDescriptorSet> writeDescriptorSetList(descriptorUpdateNum);

            for (size_t i = 0; i < descriptorUpdateNum; ++i)
            {
                BX_DESCRIPTOR_TYPE descriptorType = descriptorUpdateData[i].descriptorType;

                writeDescriptorSetList[i].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeDescriptorSetList[i].descriptorType  = Utility::VulkanUtility::GetVkDescriptorType(descriptorType);
                writeDescriptorSetList[i].descriptorCount = 1;
                writeDescriptorSetList[i].dstSet          =
                    m_descriptors[0].m_descriptorSets[descriptorUpdateData[i].descriptorSetIndex];
                writeDescriptorSetList[i].dstBinding      = descriptorUpdateData[i].descriptorBindingIndex;
                writeDescriptorSetList[i].dstArrayElement = 0;

                switch (descriptorType)
                {
                    case BX_UNIFORM_DESCRIPTOR:
                    {
                        VkDescriptorBufferInfo descriptorBufferInfo = {};
                        descriptorBufferInfo.buffer = descriptorUpdateData[i].pDescriptorBuffer->GetBuffer();
                        descriptorBufferInfo.offset = 0;
                        descriptorBufferInfo.range  = descriptorUpdateData[i].pDescriptorBuffer->GetBufferSize();

                        writeDescriptorSetList[i].pBufferInfo = &descriptorBufferInfo;

                        break;
                    }
                    case BX_SAMPLER_DESCRIPTOR:
                    {
                        VkDescriptorImageInfo descriptorImageInfo = {};
                        descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        descriptorImageInfo.imageView   =
                            descriptorUpdateData[i].pDescriptorTexture->GetTextureImageView();
                        descriptorImageInfo.sampler     =
                            descriptorUpdateData[i].pDescriptorTexture->GetTextureSampler();

                        writeDescriptorSetList[i].pImageInfo = &descriptorImageInfo;

                        break;
                    }
                    case BX_STORAGE_BUFFER_DESCRIPTOR:
                    {
                        NotImplemented();
                        break;
                    }
                    default:
                        NotSupported();
                        break;
                }
            }

            vkUpdateDescriptorSets(*m_pDevice,
                                   static_cast<UINT>(writeDescriptorSetList.size()),
                                   writeDescriptorSetList.data(),
                                   0, NULL);

            return result;
        }
    }
}
