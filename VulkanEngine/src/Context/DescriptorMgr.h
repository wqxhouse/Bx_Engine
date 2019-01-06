//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../Buffer/VulkanDescriptorBuffer.h"

namespace VulkanEngine
{
    namespace Mgr
    {
        struct Descriptor
        {
            std::vector<VkDescriptorSet> m_descriptorSets;
        };

        /*struct DescriptorUpdateInfo
        {
            
        };*/

        struct DescriptorPoolCreateInfo
        {
            BX_DESCRIPTOR_TYPE descriptorType;
            UINT               descriptorNum;
        };

        class DescriptorMgr
        {
        public:
            explicit DescriptorMgr(
                const VkDevice* const pDevice);

            ~DescriptorMgr();

            BOOL createDescriptorPool(
                const std::vector<DescriptorPoolCreateInfo>& descriptorPoolCreateData,
                const UINT                                   descriptorMaxSet);

            BOOL createDescriptorSets(
                const BX_DESCRIPTOR_TYPE              descriptorType,
                const Buffer::VulkanDescriptorBuffer* pDescriptorBuffer,
                const UINT                            descriptorSetNum);

            BOOL updateUniformDescriptorSet(
                const BX_DESCRIPTOR_TYPE              descriptorType,
                const Buffer::VulkanDescriptorBuffer* pDescriptorBuffer,
                const UINT                            descriptorSetIndex);

            inline const VkDescriptorSet GetDescriptorSet(
                const BX_DESCRIPTOR_TYPE descriptorType,
                const UINT               descriptorIndex) const
            {
                return m_descriptors[descriptorType].m_descriptorSets[descriptorIndex];
            }

            inline const std::vector<VkDescriptorSet>& GetDescriptorSetList(
                const BX_DESCRIPTOR_TYPE descriptorType) const
            {
                return m_descriptors[descriptorType].m_descriptorSets;
            }

        private:
            const VkDevice* m_pDevice;

            VDeleter<VkDescriptorPool> m_descriptorPool;

            std::vector<Descriptor> m_descriptors;
        };
    }
}