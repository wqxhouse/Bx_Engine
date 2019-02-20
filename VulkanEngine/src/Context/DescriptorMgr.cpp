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
            const VkDevice* const pDevice,
            const UINT            maxDescriptorSet)
            : m_pDevice(pDevice),
              m_maxDescriptorSet(maxDescriptorSet)
        {
            m_descriptorPool = { *pDevice, vkDestroyDescriptorPool };

            m_descriptorSetList.resize(static_cast<size_t>(maxDescriptorSet));
        }

        DescriptorMgr::~DescriptorMgr()
        {
        }

        BOOL DescriptorMgr::createDescriptorPool(
            const std::vector<DescriptorPoolCreateInfo>& descriptorPoolCreateData)
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
            descriptorPoolCreateInfo.maxSets       = m_maxDescriptorSet;

            VkResult descriptorPoolCreateResult =
                vkCreateDescriptorPool(*m_pDevice,
                                       &descriptorPoolCreateInfo,
                                       NULL,
                                       m_descriptorPool.replace());

            result = Utility::VulkanUtility::GetBxStatus(descriptorPoolCreateResult);

            assert(result == BX_SUCCESS);

            return result;
        }

        VkDescriptorSetLayout DescriptorMgr::createDescriptorSetLayout(
            const std::vector<DescriptorCreateInfo>& descriptorsCreateInfo)
        {
            BOOL result = BX_SUCCESS;

            size_t descriptorSetLayoutNum = descriptorsCreateInfo.size();

            VkDescriptorSetLayout descriptorSetLayout;
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
                                            &descriptorSetLayout);

            result = Utility::VulkanUtility::GetBxStatus(descriptorSetCreateResult);

            assert(result == BX_SUCCESS);

            return descriptorSetLayout;
        }


        BOOL DescriptorMgr::createDescriptorSets(
            const VkDescriptorSetLayout descriptorSetLayout,
            const std::vector<UINT>&    descriptorSetIndexList)
        {
            BOOL result = BX_SUCCESS;

            size_t descriptorSetNum = descriptorSetIndexList.size();
            std::vector<VkDescriptorSet>       descriptorSets(descriptorSetNum);
            std::vector<VkDescriptorSetLayout> descriptorSetLayoutList(descriptorSetNum, descriptorSetLayout);

            VkDescriptorSetAllocateInfo descriptorSetAllocInfo = {};
            descriptorSetAllocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            descriptorSetAllocInfo.descriptorPool     = m_descriptorPool;
            descriptorSetAllocInfo.pSetLayouts        = descriptorSetLayoutList.data();
            descriptorSetAllocInfo.descriptorSetCount = static_cast<UINT>(descriptorSetNum);

            VkResult allocateDescriptorSetsResult =
                vkAllocateDescriptorSets(*m_pDevice,
                                         &descriptorSetAllocInfo,
                                         descriptorSets.data());

            result = Utility::VulkanUtility::GetBxStatus(allocateDescriptorSetsResult);

            assert(result == BX_SUCCESS);

            // Updating set and set layout info
            for (size_t i = 0; i < descriptorSetNum; ++i)
            {
                UINT descriptorSetIndex = descriptorSetIndexList[i];
                m_descriptorSetList[descriptorSetIndex] = descriptorSets[i];
            }

            return result;
        }

        BOOL DescriptorMgr::createDescriptorSets(
            const std::vector<VkDescriptorSetLayout>& descriptorSetLayoutList,
            const std::vector<UINT>&                  descriptorSetIndexList)
        {
            assert(descriptorSetLayoutList.size() == descriptorSetIndexList.size());

            BOOL result = BX_SUCCESS;

            size_t descriptorSetNum = descriptorSetLayoutList.size();
            std::vector<VkDescriptorSet> descriptorSets(descriptorSetNum);

            VkDescriptorSetAllocateInfo descriptorSetAllocInfo = {};
            descriptorSetAllocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            descriptorSetAllocInfo.descriptorPool     = m_descriptorPool;
            descriptorSetAllocInfo.pSetLayouts        = descriptorSetLayoutList.data();
            descriptorSetAllocInfo.descriptorSetCount = static_cast<UINT>(descriptorSetNum);

            VkResult allocateDescriptorSetsResult =
                vkAllocateDescriptorSets(*m_pDevice,
                                         &descriptorSetAllocInfo,
                                         descriptorSets.data());

            result = Utility::VulkanUtility::GetBxStatus(allocateDescriptorSetsResult);

            assert(result == BX_SUCCESS);

            // Updating set and set layout info
            for (size_t i = 0; i < descriptorSetNum; ++i)
            {
                UINT descriptorSetIndex                       = descriptorSetIndexList[i];
                m_descriptorSetList[descriptorSetIndex]       = descriptorSets[i];
            }

            return result;
        }
        
        BOOL DescriptorMgr::updateDescriptorSet(
            const std::vector<DescriptorUpdateInfo>& descriptorSetUpdateInfo)
        {
            BOOL result = BX_SUCCESS;
            
            size_t descriptorUpdateNum = descriptorSetUpdateInfo.size();

            std::vector<VkWriteDescriptorSet> writeDescriptorSetList(descriptorUpdateNum);

            for (size_t i = 0; i < descriptorUpdateNum; ++i)
            {
                BX_DESCRIPTOR_TYPE descriptorType = descriptorSetUpdateInfo[i].descriptorType;

                writeDescriptorSetList[i].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeDescriptorSetList[i].descriptorType  = Utility::VulkanUtility::GetVkDescriptorType(descriptorType);
                writeDescriptorSetList[i].descriptorCount = 1;
                writeDescriptorSetList[i].dstSet          =
                    m_descriptorSetList[descriptorSetUpdateInfo[i].descriptorSetIndex];
                writeDescriptorSetList[i].dstBinding      =
                    descriptorSetUpdateInfo[i].descriptorBindingIndex;
                writeDescriptorSetList[i].dstArrayElement = 0;

                switch (descriptorType)
                {
                    case BX_UNIFORM_DESCRIPTOR:
                    {
                        VkDescriptorBufferInfo descriptorBufferInfo = {};
                        descriptorBufferInfo.buffer = descriptorSetUpdateInfo[i].pDescriptorBuffer->GetBuffer();
                        descriptorBufferInfo.offset = 0;
                        descriptorBufferInfo.range  = descriptorSetUpdateInfo[i].pDescriptorBuffer->GetBufferSize();

                        writeDescriptorSetList[i].pBufferInfo = &descriptorBufferInfo;

                        break;
                    }
                    case BX_UNIFORM_DESCRIPTOR_DYNAMIC:
                    {
                        // TODO: Just updating partial of dynamic buffer
                        VkDescriptorBufferInfo descriptorBufferInfo = {};
                        descriptorBufferInfo.buffer = descriptorSetUpdateInfo[i].pDescriptorBuffer->GetBuffer();
                        descriptorBufferInfo.offset = 0;
                        descriptorBufferInfo.range  = descriptorSetUpdateInfo[i].pDescriptorBuffer->GetDescriptorObjectSize();

                        writeDescriptorSetList[i].pBufferInfo = &descriptorBufferInfo;

                        break;
                    }
                    case BX_SAMPLER_DESCRIPTOR:
                    {
                        VkDescriptorImageInfo descriptorImageInfo = {};
                        descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        descriptorImageInfo.imageView   =
                            descriptorSetUpdateInfo[i].pDescriptorTexture->GetTextureImageView();
                        descriptorImageInfo.sampler     =
                            descriptorSetUpdateInfo[i].pDescriptorTexture->GetTextureSampler();

                        writeDescriptorSetList[i].pImageInfo = &descriptorImageInfo;

                        break;
                    }
                    case BX_INPUT_ATTACHMENT_DESCRIPTOR:
                    {
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
