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
            VDeleter<VkDescriptorPool>   m_descriptorPool;
            std::vector<VkDescriptorSet> m_descriptorSets;
        };

        struct DescriptorUpdateInfo
        {
            
        };

        class DescriptorMgr
        {
        public:
            DescriptorMgr(
                const VkDevice* const pDevice);

            ~DescriptorMgr();

            BOOL createDescriptorPool(
                const BX_DESCRIPTOR_TYPE descriptorType,
                const UINT               descriptorNum,
                const UINT               descriptorMaxSet);

            BOOL createDescriptorSet(
                const BX_DESCRIPTOR_TYPE              descriptorType,
                const Buffer::VulkanDescriptorBuffer* pDescriptorBuffer,
                const UINT                            descriptorSetNum);

            BOOL updateUniformDescriptorSet(
                const BX_DESCRIPTOR_TYPE              descriptorType,
                const Buffer::VulkanDescriptorBuffer* pDescriptorBuffer,
                const UINT                            descriptorSetIndex);

        private:
            const VkDevice* m_pDevice;

            std::vector<Descriptor> m_descriptors;
        };
    }
}