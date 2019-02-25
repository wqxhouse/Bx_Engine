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
            const VkDevice* const    pDevice,
            Mgr::CmdBufferMgr* const pCmdBufferMgr,
            std::vector<UINT>* const pIndexBufferData)
            : VulkanBufferBase(pDevice, pCmdBufferMgr),
              m_pIndexBufferData(pIndexBufferData)
        {
            assert(m_pIndexBufferData->size() > 0);

            determineIndexType();
        }

        VulkanIndexBuffer::VulkanIndexBuffer(
            const VkDevice* const                      pDevice,
            Mgr::CmdBufferMgr* const                   pCmdBufferMgr,
            const std::shared_ptr<Object::Model::Mesh> pMesh)
            : VulkanBufferBase(pDevice, pCmdBufferMgr)
        {
            m_pIndexBufferData = &(pMesh->m_indexBuffer);

            assert(m_pIndexBufferData->size() > 0);

            determineIndexType();
        }

        VulkanIndexBuffer::~VulkanIndexBuffer()
        {
        }

        BOOL VulkanIndexBuffer::createIndexBuffer(
            const VkPhysicalDevice& hwDevice,
            const BOOL              optimize)
        {
            BOOL result = BX_SUCCESS;

            BxBufferCreateInfo indexBufferCreateInfo = {};
            indexBufferCreateInfo.bufferUsage        = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            indexBufferCreateInfo.bufferSize         =
                (m_indexType == BX_INDEX_UINT32) ?
                sizeof(UINT) * static_cast<UINT>(m_pIndexBufferData->size()) :
                sizeof(UINT) * static_cast<UINT16>(m_pIndexBufferData->size());
            indexBufferCreateInfo.bufferData         = m_pIndexBufferData->data();
            indexBufferCreateInfo.bufferOptimization = optimize;
            indexBufferCreateInfo.bufferDynamic      = FALSE;

            result = createBuffer(hwDevice, indexBufferCreateInfo);

            assert(result == BX_SUCCESS);

            return result;
        }

        void VulkanIndexBuffer::determineIndexType()
        {
            if (typeid(m_pIndexBufferData->at(0)) == typeid(uint32_t))
            {
                m_indexType = BX_INDEX_UINT32;
            }
            else if (typeid(m_pIndexBufferData->at(0)) == typeid(uint16_t))
            {
                m_indexType = BX_INDEX_UINT16;
            }
            else
            {
                printf("Unsupported index format!\n");
                assert(FALSE);
            }
        }
    }
}
