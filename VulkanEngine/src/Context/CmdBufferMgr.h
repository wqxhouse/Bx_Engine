//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../Buffer/CmdBuffer.h"

class CmdBufferMgr
{
public:
    CmdBufferMgr(
        const VkDevice& device,
        const UINT      queueFamilyNum);

    ~CmdBufferMgr();

    BOOL createCmdBufferPool(
        const BX_QUEUE_TYPE queueType,
        const UINT          queueFamilyIndex);

    BOOL addGraphicsCmdBuffers(
        const BX_QUEUE_TYPE          queueType,
        const BX_COMMAND_BUFFER_LEVLE level,
        const UINT                   size);

    inline CmdBuffer* GetCmdBuffer(
        const BX_COMMAND_BUFFER_TYPE type,
        const UINT                   index)
    {
        CmdBuffer* pCmdBuf = NULL;

        switch (type)
        {
            case BX_GRAPHICS_COMMAND_BUFFER:
                pCmdBuf = &(m_graphicsCmdBuffers[index]);
                break;
            case BX_COMPUTE_COMMAND_BUFFER:
                pCmdBuf = &(m_computeCmdBuffers[index]);
                break;
            default:
                printf("Unsupported command buffer type!\n");
                assert(FALSE);
                break;
        }

        return pCmdBuf;
    }

private:
    const VkDevice& m_device;

    std::vector<VDeleter<VkCommandPool>> m_cmdPool;

    std::vector<CmdBuffer> m_graphicsCmdBuffers;
    std::vector<CmdBuffer> m_computeCmdBuffers;
};