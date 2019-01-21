//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../Core/VulkanUtility.h"
#include "../Context/CmdBufferMgr.h"
#include "BufferInfo.h"

namespace VulkanEngine
{
    namespace Buffer
    {
        class VulkanBufferBase
        {
        public:
            explicit VulkanBufferBase(
                const VkDevice* const    pDevice,
                Mgr::CmdBufferMgr* const pCmdBufferMgr);

            ~VulkanBufferBase();

            BOOL createBuffer(
                const VkPhysicalDevice&   hwDevice,
                const BxBufferCreateInfo& bufferCreateInfo);

            BOOL copyBuffer(
                const VkBuffer&         srcBuffer,
                const VkBuffer&         dstBuffer,
                const BxBufferCopyInfo& copyInfo);

            BOOL updateBufferData(
                const VkDeviceSize bufferSize,
                const void* const  bufferData);

            INLINE const VkBuffer GetBuffer() const
            {
                return ((m_enableOptimization == TRUE) ? m_gpuBuffer : m_hostBuffer);
            }

        protected:
            inline const VkDevice* const GetDevice() const { return m_pDevice; }

        private:
            BOOL updateHostBufferData(
                const VkDeviceSize bufferSize,
                const void* const  bufferData);

            BOOL createRawBuffer(
                const VkPhysicalDevice&      hwDevice,
                const BxRawBufferCreateInfo& rawBufferCreateInfo);

            const VkDevice*    const m_pDevice;
            Mgr::CmdBufferMgr* const m_pCmdBufferMgr;

            VDeleter<VkBuffer>       m_hostBuffer;
            VDeleter<VkDeviceMemory> m_hostBufferMemory;

            VDeleter<VkBuffer>       m_gpuBuffer;
            VDeleter<VkDeviceMemory> m_gpuBufferMemory;

            BOOL m_enableOptimization; // If enable staging buffer opt
        };
    }
}