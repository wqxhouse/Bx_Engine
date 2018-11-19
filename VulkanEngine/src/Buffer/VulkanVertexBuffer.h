#pragma once

#include <Model/Mesh/Mesh.h>

#include "../Core/VulkanPCH.h"
#include "../Core/VulkanUtility.h"

class VulkanVertexBuffer
{
public:
    VulkanVertexBuffer(
        const VkDevice&                             device,
        const std::unique_ptr<Object::Model::Mesh>& pMesh);

    VkVertexInputBindingDescription createDescription(
        const UINT                 binding,
        const BX_VERTEX_INPUT_RATE rate);

    std::array<VkVertexInputAttributeDescription, 3> createAttributeDescriptions();

    ~VulkanVertexBuffer();

private:
    std::shared_ptr<std::vector<Object::Model::Vertex>> m_vertexBufferData;

    VDeleter<VkBuffer>       m_vertexBuffer;
    VDeleter<VkDeviceMemory> m_vertexBufferMemory;
};