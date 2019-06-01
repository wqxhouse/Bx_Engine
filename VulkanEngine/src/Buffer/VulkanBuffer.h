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
        struct VulkanBufferRangeData
        {
            UINT  offset;
            void* data;
        };

        class VulkanBufferBase
        {
        public:
            explicit VulkanBufferBase(
                const VkDevice* const    pDevice,
                Mgr::CmdBufferMgr* const pCmdBufferMgr);

            virtual ~VulkanBufferBase();

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
                return ((IsBufferOptimized() == TRUE) ? m_gpuBuffer : m_hostBuffer);
            }

            INLINE const BOOL IsBufferOptimized() const
            {
                return m_bufferFlags.bits.enableOptimization;
            }

            INLINE const BOOL IsBufferDynamic() const
            {
                return m_bufferFlags.bits.dynamicBuffer;
            }

            VDeleter<VkBuffer>       m_hostBuffer;
            VDeleter<VkDeviceMemory> m_hostBufferMemory;

            VDeleter<VkBuffer>       m_gpuBuffer;
            VDeleter<VkDeviceMemory> m_gpuBufferMemory;

        protected:
            BOOL updateBufferDataRange(
                const VkDeviceSize                        bufferSize,
                const VkDeviceSize                        elementSize,
                const std::vector<VulkanBufferRangeData>& offsets);

            INLINE const VkDevice* const GetDevice() const { return m_pDevice; }

        private:
            BOOL updateHostBufferData(
                const VkDeviceSize bufferSize,
                const void* const  bufferData);

            BOOL createRawBuffer(
                const VkPhysicalDevice&      hwDevice,
                const BxRawBufferCreateInfo& rawBufferCreateInfo);

            const VkDevice*    const m_pDevice;
            Mgr::CmdBufferMgr* const m_pCmdBufferMgr;

            union VulkanBufferFlags
            {
                UINT value;

                struct
                {
                    BOOL enableOptimization : 1; // If enable staging buffer opt
                    BOOL dynamicBuffer      : 1; // If the buffer can be dynamic updated
                    UINT reserve            : 30;
                } bits;
            } m_bufferFlags;
        };
    }
}