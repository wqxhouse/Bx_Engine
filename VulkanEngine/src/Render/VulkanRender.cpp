//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "VulkanRender.h"

namespace VulkanEngine
{
    namespace Render
    {
        VulkanRenderBase::VulkanRenderBase(
            const Setting*                                const pSetting,
            const VkPhysicalDevice*                       const pHwDevice,
            const VkDevice*                               const pDevice,
            Mgr::CmdBufferMgr*                            const pCmdBufferMgr,
            Mgr::DescriptorMgr*                           const pDescritorMgr,
            Mgr::TextureMgr*                              const pTextureMgr,
            const Scene::RenderScene*                           pScene,
            const std::vector<Texture::VulkanTexture2D*>* const ppBackbufferTextures)
            : m_pSetting(pSetting),
              m_pHwDevice(pHwDevice),
              m_pDevice(pDevice),
              m_pCmdBufferMgr(pCmdBufferMgr),
              m_pDescritorMgr(pDescritorMgr),
              m_pTextureMgr(pTextureMgr),
              m_pScene(pScene),
              m_mainSceneRenderPass(pSetting, pDevice, pCmdBufferMgr, pDescritorMgr, pScene),
              m_ppBackbufferTextures(ppBackbufferTextures)
        {
            parseScene();
        }

        VulkanRenderBase::~VulkanRenderBase()
        {
            for (VulkanVertexInputResource vertexInputResources : m_mainSceneVertexInputResourceList)
            {
                delete vertexInputResources.pIndexBuffer;
                delete vertexInputResources.pVertexBuffer;
            }
        }

        void VulkanRenderBase::parseScene()
        {
            // Parse Models
            const UINT modelNum = m_pScene->GetSceneModelNum();

            for (UINT i = 0; i < modelNum; ++i)
            {
                const Object::Model::ModelObject* pModel = m_pScene->GetModel(i);

                const UINT meshNum = pModel->GetMeshNum();

                for (UINT j = 0; j < meshNum; ++j)
                {
                    if (pModel->IsEnable())
                    {
                        Buffer::VulkanVertexBuffer* pVertexBuffer = new Buffer::VulkanVertexBuffer(
                            m_pDevice, m_pCmdBufferMgr, pModel->GetMesh(j));
                        pVertexBuffer->createVertexBuffer(*m_pHwDevice, TRUE);

                        Buffer::VulkanIndexBuffer* pIndexBuffer = new Buffer::VulkanIndexBuffer(
                            m_pDevice, m_pCmdBufferMgr, pModel->GetMesh(j));
                        pIndexBuffer->createIndexBuffer(*m_pHwDevice, TRUE);

                        m_mainSceneVertexInputResourceList.push_back({ pVertexBuffer, pIndexBuffer });
                    }
                }
            }
        }
    }
}