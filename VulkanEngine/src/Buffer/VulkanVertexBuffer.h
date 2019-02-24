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
        struct VertexInputBindingCreateData
        {
            UINT                 bindingPoint;
            BX_VERTEX_INPUT_RATE rate;
        };

        struct VertexInputAttributeCreateData
        {
            UINT     bindingPoint;
            UINT     location;
            VkFormat format;
            UINT     offset;
        };

        class VulkanVertexBuffer : public VulkanBufferBase
        {
        public:
            VulkanVertexBuffer(
                const VkDevice* const    pDevice,
                Mgr::CmdBufferMgr* const pCmdBufferMgr,
                const UINT               vertexNum,
                const UINT               vertexSize,
                void*                    pVertexBufferData);

            VulkanVertexBuffer(
                const VkDevice*    const                   pDevice,
                Mgr::CmdBufferMgr* const                   pCmdBufferMgr,
                const std::shared_ptr<Object::Model::Mesh> pMesh);

            BOOL createVertexBuffer(
                const VkPhysicalDevice& hwDevice,
                const BOOL              optimize);

            static std::vector<VertexInputAttributeCreateData>
                GenPosQuadAttributeListCreateData(
                    const UINT bindingPoint);

            static std::vector<VertexInputAttributeCreateData>
                GenSingleTextureChannelMeshAttributeListCreateData(
                    const UINT bindingPoint);

            static std::vector<VertexInputAttributeCreateData>
                GenMultipleTextureChannelMeshAttributeListCreatData(
                    const UINT bindingPoint,
                    const UINT texChannelNum);

            static VkVertexInputBindingDescription CreateDescription(
                const UINT                 bindingPoint,
                const UINT                 vertexSize,
                const BX_VERTEX_INPUT_RATE rate);

            static std::vector<VkVertexInputAttributeDescription> CreateAttributeDescriptions(
                const std::vector<VertexInputAttributeCreateData>& vertexAttributeCreateDataList);

            INLINE const VkBuffer GetVertexBuffer() const
            {
                return GetBuffer();
            }

            INLINE const VkVertexInputBindingDescription GetVertexInputBindingDescription() const
            {
                return m_bindingDescription;
            }

            INLINE const std::vector<VkVertexInputAttributeDescription> GetVertexInputAttributeDescriptionList() const
            {
                return m_vertexInputAttributeList;
            }

            INLINE const UINT GetVertexNum() const
            {
                return m_vertexBufferNum;
            }

            INLINE const UINT GetVertexSize() const
            {
                return m_vertexSize;
            }

            INLINE const UINT GetVertexBufferSize() const
            {
                return m_vertexBufferSize;
            }

            ~VulkanVertexBuffer();

        private:
            VkVertexInputBindingDescription                m_bindingDescription;
            std::vector<VkVertexInputAttributeDescription> m_vertexInputAttributeList;

            UINT  m_vertexBufferNum;
            UINT  m_vertexSize;
            UINT  m_vertexBufferSize;

            void* m_pVertexBufferData;
        };
    }
}