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
#include "../Texture/VulkanTexture.h"

namespace VulkanEngine
{
    namespace Mgr
    {
        struct DescriptorCreateInfo
        {
            BX_DESCRIPTOR_TYPE descriptorType;
            BX_SHADER_TYPE     shaderType;
            UINT               bindingPoint;
            UINT               descriptorNum;
        };

        struct DescriptorPoolCreateInfo
        {
            BX_DESCRIPTOR_TYPE descriptorType;
            UINT               descriptorNum;
        };

        struct DescriptorUpdateInfo
        {
            BX_DESCRIPTOR_TYPE              descriptorType;
            Buffer::VulkanDescriptorBuffer* pDescriptorBuffer;
            Texture::VulkanTextureBase*     pDescriptorTexture;
            UINT                            descriptorSetIndex;
            UINT                            descriptorBindingIndex;
        };

        class DescriptorMgr
        {
        public:
            explicit DescriptorMgr(
                const VkDevice* const pDevice,
                const UINT            maxDescriptorSet);

            ~DescriptorMgr();

            BOOL createDescriptorPool(
                const std::vector<DescriptorPoolCreateInfo>& descriptorPoolCreateData);

            BOOL createDescriptorSets(
                const VkDescriptorSetLayout descriptorSetLayout,
                const std::vector<UINT>&    descriptorSetIndexList);

            BOOL createDescriptorSets(
                const std::vector<VkDescriptorSetLayout>& descriptorSetLayoutList,
                const std::vector<UINT>&                  descriptorSetIndexList);

            BOOL updateDescriptorSet(
                const std::vector<DescriptorUpdateInfo>& descriptorUpdateData);

            VkDescriptorSetLayout createDescriptorSetLayout(
                const std::vector<DescriptorCreateInfo>& descriptorsCreateInfo);

            INLINE const VkDescriptorSet GetDescriptorSet(
                const UINT descriptorSetIndex) const
            {
                return m_descriptorSetList[descriptorSetIndex];
            }

        private:
            const VkDevice*              m_pDevice;

            VDeleter<VkDescriptorPool>   m_descriptorPool;
            std::vector<VkDescriptorSet> m_descriptorSetList;

            UINT                         m_maxDescriptorSet;
        };
    }
}