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

#include "../Core/VulkanPCH.h"
#include "../Core/VulkanUtility.h"

class VulkanVertexBuffer
{
public:
    VulkanVertexBuffer(
        const VkDevice* const                      pDevice,
        const std::shared_ptr<Object::Model::Mesh> pMesh);

    BOOL createVulkanVertexBuffer(
        const VkPhysicalDevice* const pHwDevice);

    VkVertexInputBindingDescription createDescription(
        const UINT                 binding,
        const BX_VERTEX_INPUT_RATE rate);

    std::array<VkVertexInputAttributeDescription, 3> createAttributeDescriptions();

    inline const VkBuffer GetVertexBuffer() const
    {
        return m_vertexBuffer;
    }

    ~VulkanVertexBuffer();

private:
    const VkDevice* const m_pDevice;

    std::vector<Object::Model::Vertex>* m_vertexBufferData;

    VDeleter<VkBuffer>       m_vertexBuffer;
    VDeleter<VkDeviceMemory> m_vertexBufferMemory;
};