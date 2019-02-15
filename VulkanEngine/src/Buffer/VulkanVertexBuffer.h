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
        class VulkanVertexBuffer : public VulkanBufferBase
        {
        public:
            VulkanVertexBuffer(
                const VkDevice* const    pDevice,
                Mgr::CmdBufferMgr* const pCmdBufferMgr,
                const UINT               vertexBufferDataSize,
                void*                    pVertexBufferData);

            VulkanVertexBuffer(
                const VkDevice*    const                   pDevice,
                Mgr::CmdBufferMgr* const                   pCmdBufferMgr,
                const std::shared_ptr<Object::Model::Mesh> pMesh);

            BOOL createVertexBuffer(
                const VkPhysicalDevice& hwDevice,
                const BOOL              optimize);

            static VkVertexInputBindingDescription createDescription(
                const UINT                 bindingPoint,
                const BX_VERTEX_INPUT_RATE rate);

            static std::vector<VkVertexInputAttributeDescription>
                createAttributeDescriptions(
                    const UINT bindingPoint);

            static std::vector<VkVertexInputAttributeDescription>
                createAttributeDescriptionsMultipleTexture(
                    const UINT bindingPoint,
                    const UINT texChannels);

            INLINE const VkBuffer GetVertexBuffer() const
            {
                return GetBuffer();
            }

            INLINE UINT GetVertexNum() const
            {
                return m_vertexBufferSize;
            }

            INLINE UINT GetVertexBufferSize() const
            {
                return GetVertexNum() * sizeof(Object::Model::Vertex);
            }

            ~VulkanVertexBuffer();

        private:
            UINT  m_vertexBufferSize;
            void* m_pVertexBufferData;
        };
    }
}