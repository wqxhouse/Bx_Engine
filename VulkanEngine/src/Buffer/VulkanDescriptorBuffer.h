//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "VulkanBuffer.h"

namespace VulkanEngine
{
    namespace Buffer
    {
        class VulkanDescriptorBuffer : public VulkanBufferBase
        {
        public:
            VulkanDescriptorBuffer(
                const VkDevice*    const pDevice,
                Mgr::CmdBufferMgr* const pCmdBufferMgr);

            ~VulkanDescriptorBuffer();

            virtual BOOL createDescriptorSetLayout(
                const UINT               bindingPoint,
                const UINT               descriptorNum,
                const VkShaderStageFlags stageFlags) = 0;

            inline const VkDescriptorSetLayout GetDescriptorSetLayout() const
            {
                return m_descriptorSetLayout;
            }

            inline const UINT64 GetBufferSize() const { return m_bufferSize; }

        protected:
            VDeleter<VkDescriptorSetLayout> m_descriptorSetLayout;

            UINT64 m_bufferSize;
        };
    }
}