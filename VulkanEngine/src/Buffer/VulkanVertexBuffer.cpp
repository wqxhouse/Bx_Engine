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
            const VkDevice* const    pDevice,
            Mgr::CmdBufferMgr* const pCmdBufferMgr,
            const UINT               vertexNum,
            const UINT               vertexSize,
            void*                    pVertexBufferData)
            : VulkanBufferBase(pDevice, pCmdBufferMgr),
              m_vertexBufferNum(vertexNum),
              m_vertexSize(vertexSize),
              m_pVertexBufferData(pVertexBufferData)
        {
            m_vertexBufferSize = vertexNum * vertexSize;
        }

        VulkanVertexBuffer::VulkanVertexBuffer(
            const VkDevice* const       pDevice,
            Mgr::CmdBufferMgr* const    pCmdBufferMgr,
            const std::shared_ptr<Mesh> pMesh)
            : VulkanBufferBase(pDevice, pCmdBufferMgr)
        {
            m_vertexBufferSize  = static_cast<UINT>(sizeof(Vertex) * pMesh->m_vertexBuffer.size());
            m_pVertexBufferData = static_cast<void*>(pMesh->m_vertexBuffer.data());
        }

        BOOL VulkanVertexBuffer::createVertexBuffer(
            const VkPhysicalDevice& hwDevice,
            const BOOL              optimize)
        {
            BOOL result = BX_SUCCESS;

            // Create vertex buffer
            BxBufferCreateInfo bufferCreateInfo = {};
            bufferCreateInfo.bufferUsage        = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            bufferCreateInfo.bufferSize         = static_cast<VkDeviceSize>(GetVertexBufferSize());
            bufferCreateInfo.bufferData         = m_pVertexBufferData;
            bufferCreateInfo.bufferOptimization = optimize;
            bufferCreateInfo.bufferDynamic      = FALSE;

            result = createBuffer(hwDevice, bufferCreateInfo);

            assert(result == BX_SUCCESS);

            return result;
        }

        std::vector<VertexInputAttributeCreateData>
        VulkanVertexBuffer::GenPosQuadAttributeListCreateData(
            const UINT bindingPoint)
        {
            std::vector<VertexInputAttributeCreateData> attributesCreateData(1);

            attributesCreateData[0].bindingPoint = bindingPoint;
            attributesCreateData[0].location     = 0;
            attributesCreateData[0].format       = VK_FORMAT_R32G32_SFLOAT;
            attributesCreateData[0].offset       = 0;

            return attributesCreateData;
        }

        std::vector<VertexInputAttributeCreateData>
        VulkanVertexBuffer::GenSingleTextureChannelMeshAttributeListCreateData(
            const UINT bindingPoint)
        {
            std::vector<VertexInputAttributeCreateData> attributesCreateData(3);

            attributesCreateData[0].bindingPoint = bindingPoint;
            attributesCreateData[0].location     = 0;
            attributesCreateData[0].format       = VK_FORMAT_R32G32B32_SFLOAT;
            attributesCreateData[0].offset       = static_cast<UINT>(offsetof(Vertex, Vertex::position));

            attributesCreateData[1].bindingPoint = bindingPoint;
            attributesCreateData[1].location     = 1;
            attributesCreateData[1].format       = VK_FORMAT_R32G32B32_SFLOAT;
            attributesCreateData[1].offset       = static_cast<UINT>(offsetof(Vertex, Vertex::normal));

            attributesCreateData[2].bindingPoint = bindingPoint;
            attributesCreateData[2].location     = 2;
            attributesCreateData[2].format       = VK_FORMAT_R32G32_SFLOAT;
            attributesCreateData[2].offset       = static_cast<UINT>(offsetof(Vertex, Vertex::texCoords));

            return attributesCreateData;
        }

        std::vector<VertexInputAttributeCreateData>
        VulkanVertexBuffer::GenMultipleTextureChannelMeshAttributeListCreatData(
            const UINT bindingPoint,
            const UINT texChannelNum)
        {
            std::vector<VertexInputAttributeCreateData> attributesCreateData(texChannelNum + 2);

            attributesCreateData[0].bindingPoint = bindingPoint;
            attributesCreateData[0].location     = 0;
            attributesCreateData[0].format       = VK_FORMAT_R32G32B32_SFLOAT;
            attributesCreateData[0].offset       = static_cast<UINT>(offsetof(Vertex, Vertex::position));

            attributesCreateData[1].bindingPoint = bindingPoint;
            attributesCreateData[1].location     = 1;
            attributesCreateData[1].format       = VK_FORMAT_R32G32B32_SFLOAT;
            attributesCreateData[1].offset       = static_cast<UINT>(offsetof(Vertex, Vertex::normal));

            for (UINT texChannelIndex = 0; texChannelIndex < texChannelNum; ++texChannelIndex)
            {
                UINT attributeIndex = texChannelIndex + 2;

                attributesCreateData[attributeIndex].bindingPoint = bindingPoint;
                attributesCreateData[attributeIndex].location     = attributeIndex;
                attributesCreateData[attributeIndex].format       = VK_FORMAT_R32G32_SFLOAT;

                // TODO: Need to create multi-texture coordinate channel vertex struct
                NotImplemented();
                attributesCreateData[attributeIndex].offset       = static_cast<UINT>(offsetof(Vertex, Vertex::texCoords));
            }

            return attributesCreateData;
        }

        VkVertexInputBindingDescription VulkanVertexBuffer::CreateDescription(
            const UINT                 bindingPoint,
            const UINT                 vertexSize,
            const BX_VERTEX_INPUT_RATE rate)
        {
            VkVertexInputBindingDescription bindingDescription = {};
            bindingDescription.binding   = bindingPoint;
            bindingDescription.stride    = vertexSize;
            bindingDescription.inputRate = VulkanUtility::GetVkVertexInputRate(rate);

            return bindingDescription;
        }

        std::vector<VkVertexInputAttributeDescription>
        VulkanVertexBuffer::CreateAttributeDescriptions(
                const std::vector<VertexInputAttributeCreateData>& vertexAttributeCreateDataList)
        {
            size_t vertexAttributeCreateDataSize = vertexAttributeCreateDataList.size();

            std::vector<VkVertexInputAttributeDescription>
            vertexInputAttributeDescriptionList(vertexAttributeCreateDataSize);

            for (size_t vertexInputAttributeDescriptionIndex = 0;
                 vertexInputAttributeDescriptionIndex < vertexAttributeCreateDataSize;
                 ++vertexInputAttributeDescriptionIndex)
            {
                const VertexInputAttributeCreateData& vertexAttributeCreateData =
                    vertexAttributeCreateDataList[vertexInputAttributeDescriptionIndex];

                const UINT location = vertexAttributeCreateData.location;

                vertexInputAttributeDescriptionList[location].binding  = vertexAttributeCreateData.bindingPoint;
                vertexInputAttributeDescriptionList[location].location = location;
                vertexInputAttributeDescriptionList[location].format   = vertexAttributeCreateData.format;
                vertexInputAttributeDescriptionList[location].offset   = vertexAttributeCreateData.offset;
            }

            return vertexInputAttributeDescriptionList;
        }

        VulkanVertexBuffer::~VulkanVertexBuffer()
        {
        }
    }
}