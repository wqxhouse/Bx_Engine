//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "VulkanVertexBuffer.h"

using namespace Object::Model;

VulkanVertexBuffer::VulkanVertexBuffer(
    const VkDevice* const       pDevice,
    const std::shared_ptr<Mesh> pMesh)
    : m_pDevice(pDevice)
{
    m_vertexBuffer       = { *m_pDevice, vkDestroyBuffer };
    m_vertexBufferMemory = { *m_pDevice, vkFreeMemory    };

    m_vertexBufferData = &(pMesh->m_vertexBuffer);
}

BOOL VulkanVertexBuffer::createVulkanVertexBuffer()
{
    BOOL result = BX_SUCCESS;

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size        = sizeof(Vertex) * static_cast<UINT64>(m_vertexBufferData->size());
    bufferCreateInfo.usage       = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult vertexBufferCreateResult =
        vkCreateBuffer(*m_pDevice, &bufferCreateInfo, NULL, m_vertexBuffer.replace());
    
    result = VulkanUtility::GetBxStatus(vertexBufferCreateResult);

    assert(result == BX_SUCCESS);

    return result;
}

VkVertexInputBindingDescription VulkanVertexBuffer::createDescription(
    const UINT                 binding,
    const BX_VERTEX_INPUT_RATE rate)
{
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding   = binding;
    bindingDescription.stride    = sizeof(Vertex);
    bindingDescription.inputRate = VulkanUtility::GetVkVertexInputRate(rate);

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 3>
VulkanVertexBuffer::createAttributeDescriptions()
{
    std::array<VkVertexInputAttributeDescription, 3> attributes;

    attributes[0].binding  = 0;
    attributes[0].location = 0;
    attributes[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attributes[0].offset   = static_cast<UINT>(offsetof(Vertex, Vertex::position));

    attributes[1].binding  = 0;
    attributes[1].location = 1;
    attributes[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attributes[1].offset   = static_cast<UINT>(offsetof(Vertex, Vertex::normal));

    attributes[2].binding  = 0;
    attributes[2].location = 2;
    attributes[2].format   = VK_FORMAT_R32G32_SFLOAT;
    attributes[2].offset   = static_cast<UINT>(offsetof(Vertex, Vertex::texCoords));

    return attributes;
}

VulkanVertexBuffer::~VulkanVertexBuffer()
{

}