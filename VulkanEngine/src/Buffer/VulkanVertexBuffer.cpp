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
using namespace VulkanEngine::Utility;

namespace VulkanEngine
{
    namespace Buffer
    {
        VulkanVertexBuffer::VulkanVertexBuffer(
            const VkDevice* const       pDevice,
            Mgr::CmdBufferMgr* const    pCmdBufferMgr,
            const std::shared_ptr<Mesh> pMesh)
            : VulkanBufferBase(pDevice, pCmdBufferMgr)
        {
            m_pVertexBufferData = &(pMesh->m_vertexBuffer);
        }

        BOOL VulkanVertexBuffer::createVertexBuffer(
            const VkPhysicalDevice& hwDevice,
            const BOOL              optimize)
        {
            BOOL result = BX_SUCCESS;

            BxBufferCreateInfo bufferCreateInfo = {};
            bufferCreateInfo.bufferUsage        = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            bufferCreateInfo.bufferSize         =
                sizeof(Vertex) * static_cast<UINT64>(m_pVertexBufferData->size());
            bufferCreateInfo.bufferData         = static_cast<const void*>(m_pVertexBufferData->data());
            bufferCreateInfo.bufferOptimization = optimize;

            result = createBuffer(hwDevice, bufferCreateInfo);

            assert(result == BX_SUCCESS);

            return result;
        }

        VkVertexInputBindingDescription VulkanVertexBuffer::createDescription(
            const UINT                 bindingPoint,
            const BX_VERTEX_INPUT_RATE rate)
        {
            VkVertexInputBindingDescription bindingDescription = {};
            bindingDescription.binding   = bindingPoint;
            bindingDescription.stride    = static_cast<UINT>(sizeof(Vertex));
            bindingDescription.inputRate = VulkanUtility::GetVkVertexInputRate(rate);

            return bindingDescription;
        }

        std::vector<VkVertexInputAttributeDescription>
            VulkanVertexBuffer::createAttributeDescriptions(
                const UINT bindingPoint)
        {
            std::vector<VkVertexInputAttributeDescription> attributes(3);

            attributes[0].binding  = bindingPoint;
            attributes[0].location = 0;
            attributes[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
            attributes[0].offset   = static_cast<UINT>(offsetof(Vertex, Vertex::position));

            attributes[1].binding  = bindingPoint;
            attributes[1].location = 1;
            attributes[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
            attributes[1].offset   = static_cast<UINT>(offsetof(Vertex, Vertex::normal));

            attributes[2].binding  = bindingPoint;
            attributes[2].location = 2;
            attributes[2].format   = VK_FORMAT_R32G32_SFLOAT;
            attributes[2].offset   = static_cast<UINT>(offsetof(Vertex, Vertex::texCoords));

            return attributes;
        }

        std::vector<VkVertexInputAttributeDescription>
            VulkanVertexBuffer::createAttributeDescriptionsMultipleTexture(
                const UINT bindingPoint,
                const UINT texChannels)
        {
            std::vector<VkVertexInputAttributeDescription> attributes(texChannels);

            attributes[0].binding  = bindingPoint;
            attributes[0].location = 0;
            attributes[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
            attributes[0].offset   = static_cast<UINT>(offsetof(Vertex, Vertex::position));

            attributes[1].binding  = bindingPoint;
            attributes[1].location = 1;
            attributes[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
            attributes[1].offset   = static_cast<UINT>(offsetof(Vertex, Vertex::normal));

            for (UINT i = 0; i < texChannels; ++i)
            {
                UINT attributeIndex = i + 2;

                attributes[attributeIndex].binding  = bindingPoint;
                attributes[attributeIndex].location = attributeIndex;
                attributes[attributeIndex].format   = VK_FORMAT_R32G32_SFLOAT;
                attributes[attributeIndex].offset   = static_cast<UINT>(offsetof(Vertex, Vertex::texCoords));
            }

            return attributes;
        }

        VulkanVertexBuffer::~VulkanVertexBuffer()
        {
        }
    }
}