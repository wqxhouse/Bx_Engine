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
        struct BxCmdBufferCreateInfo
        {
            const VkCommandPool*    pCommandPool;
            BX_COMMAND_BUFFER_TYPE  cmdBufferType;
            BX_COMMAND_BUFFER_LEVLE bufferLevel;
        };

        struct BxBufferCreateInfo
        {
            VkBufferUsageFlags bufferUsage;
            VkDeviceSize       bufferSize;
            const void*        bufferData;
            BOOL               bufferOptimization;
        };

        struct BxImageSubresourceInfo
        {
            VkImageAspectFlagBits aspectMask;
            UINT                  baseMipLevel;
            UINT                  mipmapLevelNum;
            UINT                  baseArrayLayer;
            UINT                  layerNum;
        };

        struct BxBufferCopyInfo
        {
            VkDeviceSize copySize;
            VkDeviceSize srcOffset = 0;
            VkDeviceSize dstOffset = 0;
        };

        struct BxBufferToImageCopyInfo
        {
            struct BufferInfo
            {
                UINT bufferOffset;
                UINT bufferRowLength;
                UINT bufferImageHeight;
            } bufferInfo;

            BxImageSubresourceInfo subResourceInfo;

            struct ImageInfo
            {
                VkOffset3D imageOffset;
                VkExtent3D imageExtent;
            } imageInfo;
        };

        struct BxLayoutTransitionInfo
        {
            VkImageLayout          oldLayout;
            VkImageLayout          newLayout;
            VkAccessFlags          srcAccessMask;
            VkAccessFlags          dstAccessMask;
            BxImageSubresourceInfo subResourceInfo;
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