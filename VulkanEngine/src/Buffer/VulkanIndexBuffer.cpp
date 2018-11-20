//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "VulkanIndexBuffer.h"

namespace VulkanEngine
{
    namespace Buffer
    {
        VulkanIndexBuffer::VulkanIndexBuffer(
            const VkDevice* const                      pDevice,
            Mgr::CmdBufferMgr* const                   pCmdBufferMgr,
            const std::shared_ptr<Object::Model::Mesh> pMesh)
            : VulkanBufferBase(pDevice, pCmdBufferMgr)
        {
            m_pIndexBufferData = &(pMesh->m_indexBuffer);
        }

        BOOL VulkanIndexBuffer::createIndexBuffer(
            const VkPhysicalDevice& hwDevice,
            const BOOL              optimize)
        {
            BOOL result = BX_SUCCESS;

            BxBufferCreateInfo indexBufferCreateInfo = {};
            indexBufferCreateInfo.bufferUsage        = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            indexBufferCreateInfo.bufferSize         =
                sizeof(UINT) * static_cast<UINT>(m_pIndexBufferData->size());
            indexBufferCreateInfo.bufferData         = m_pIndexBufferData->data();
            indexBufferCreateInfo.bufferOptimization = optimize;

            result = createBuffer(hwDevice, indexBufferCreateInfo);

            assert(result == BX_SUCCESS);

            return result;
        }

        VulkanIndexBuffer::~VulkanIndexBuffer()
        {
        }
    }
}
