//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include <Model/Mesh/Mesh.h>

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

            BOOL createIndexBuffer(
                const VkPhysicalDevice& hwDevice,
                const BOOL              optimize);

            ~VulkanIndexBuffer();

        private:
            std::vector<UINT>* m_pIndexBufferData;
        };
    }
}