//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../Core/VulkanPCH.h"

namespace VulkanEngine
{
    namespace Buffer
    {
        struct BxBufferCreateInfo
        {
            VkBufferUsageFlags bufferUsage;
            VkDeviceSize       bufferSize;
            void*              bufferData;
            BOOL               bufferOptimization;
        };

        struct BxBufferCopyInfo
        {
            VkDeviceSize copySize;
            VkDeviceSize srcOffset = 0;
            VkDeviceSize dstOffset = 0;
        };

        struct BxRawBufferCreateInfo
        {
            VkBuffer*             pBuffer;
            VkDeviceMemory*       pBufferMemory;
            VkMemoryPropertyFlags bufferProperties;
            VkBufferUsageFlags    bufferUsage;
            VkDeviceSize          bufferSize;
        };
    }
}