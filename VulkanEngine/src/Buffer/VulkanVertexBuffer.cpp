#include "VulkanVertexBuffer.h"

using namespace Object::Model;

VulkanVertexBuffer::VulkanVertexBuffer(
    const VkDevice&             device,
    const std::shared_ptr<Mesh> pMesh)
{
    m_vertexBuffer       = { device, vkDestroyBuffer };
    m_vertexBufferMemory = { device, vkFreeMemory };

    m_vertexBufferData = &(pMesh->m_vertexBuffer);
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