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
            BX_DESCRIPTOR_TYPE                       descriptorType;
            Buffer::VulkanDescriptorBuffer*          pDescriptorBuffer;
            std::vector<Texture::VulkanTextureBase*> pDescriptorTextureList;
            UINT                                     descriptorSetIndex;
            UINT                                     descriptorBindingIndex;
            UINT                                     descriptorCount;
        };

        class DescriptorMgr
        {
        public:
            explicit DescriptorMgr(
                const VkDevice* const pDevice,
                CmdBufferMgr*         pCmdMgr,
                const UINT            maxDescriptorSet);

            ~DescriptorMgr();

            BOOL createDescriptorPool(
                const std::vector<DescriptorPoolCreateInfo>& descriptorPoolCreateData,
                const UINT                                   renderpassIndex);

            BOOL createDescriptorSets(
                const VkDescriptorSetLayout descriptorSetLayout,
                const std::vector<UINT>&    descriptorSetIndexList,
                const UINT                  renderpassIndex);

            BOOL createDescriptorSets(
                const std::vector<VkDescriptorSetLayout>& descriptorSetLayoutList,
                const std::vector<UINT>&                  descriptorSetIndexList,
                const UINT                                renderpassIndex);

            BOOL updateDescriptorSet(
                const std::vector<DescriptorUpdateInfo>& descriptorSetUpdateInfo);

            VkDescriptorSetLayout createDescriptorSetLayout(
                const std::vector<DescriptorCreateInfo>& descriptorsCreateInfo);

            INLINE const UINT GetDescriptorSetNum() const { return static_cast<UINT>(m_descriptorSetList.size()); }

            INLINE const VkDescriptorSet GetDescriptorSet(
                const UINT descriptorSetIndex) const
            {
                return m_descriptorSetList[descriptorSetIndex];
            }

        private:
            const VkDevice*                           m_pDevice;

            CmdBufferMgr*                             m_pCmdMgr;

            std::vector<VDeleter<VkDescriptorPool>>   m_descriptorPoolList;
            std::vector<VkDescriptorSet>              m_descriptorSetList;

            const UINT                                m_maxDescriptorSet;

            const static UINT                         m_maxDescriptorPoolNum;
        };
    }
}