//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include <Object/Model/Mesh/Mesh.h>

#include "VulkanBuffer.h"

namespace VulkanEngine
{
    namespace Buffer
    {
        class VulkanIndexBuffer : public VulkanBufferBase
        {
        public:
            VulkanIndexBuffer(
                const VkDevice* const                      pDevice,
                Mgr::CmdBufferMgr* const                   pCmdBufferMgr,
                const std::shared_ptr<Object::Model::Mesh> pMesh);

            ~VulkanIndexBuffer();

            BOOL createIndexBuffer(
                const VkPhysicalDevice& hwDevice,
                const BOOL              optimize);

            inline BX_INDEX_TYPE GetIndexType() const { return m_indexType; }

            inline UINT          GetIndexNum()  const
            {
                return static_cast<UINT>(m_pIndexBufferData->size());
            }

        private:
            BX_INDEX_TYPE      m_indexType;

            std::vector<UINT>* m_pIndexBufferData;
        };
    }
}