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
        class VulkanVertexBuffer : public VulkanBufferBase
        {
        public:
            VulkanVertexBuffer(
                const VkDevice*    const                   pDevice,
                Mgr::CmdBufferMgr* const                   pCmdBufferMgr,
                const std::shared_ptr<Object::Model::Mesh> pMesh);

            BOOL createVertexBuffer(
                const VkPhysicalDevice& hwDevice,
                const BOOL              optimize);

            static VkVertexInputBindingDescription createDescription(
                const UINT                 binding,
                const BX_VERTEX_INPUT_RATE rate);

            static std::array<VkVertexInputAttributeDescription, 3>
            createAttributeDescriptions();

            inline const VkBuffer GetVertexBuffer() const
            {
                return GetBuffer();
            }

            inline UINT GetVertexNum() const
            {
                return static_cast<UINT>(m_pVertexBufferData->size());
            }

            inline UINT GetVertexBufferSize() const
            {
                return GetVertexNum() * sizeof(Object::Model::Vertex);
            }

            ~VulkanVertexBuffer();

        private:
            std::vector<Object::Model::Vertex>* m_pVertexBufferData;
        };
    }
}