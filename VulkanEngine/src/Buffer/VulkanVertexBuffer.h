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
                const VkDevice* const                      pDevice,
                const std::shared_ptr<Object::Model::Mesh> pMesh);

            BOOL createVulkanVertexBuffer(
                const VkPhysicalDevice& hwDevice);

            VkVertexInputBindingDescription createDescription(
                const UINT                 binding,
                const BX_VERTEX_INPUT_RATE rate);

            std::array<VkVertexInputAttributeDescription, 3> createAttributeDescriptions();

            inline const VkBuffer GetVertexBuffer() const
            {
                return GetBuffer();
            }

            ~VulkanVertexBuffer();

        private:
            std::vector<Object::Model::Vertex>* m_pVertexBufferData;
        };
    }
}