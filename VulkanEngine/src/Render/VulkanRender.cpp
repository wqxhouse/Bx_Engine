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
              m_isDepthTestEnabled(FALSE),
              m_isStencilTestEnabled(FALSE)
        {
            assert(ppBackbufferTextures->size() > 0);

            size_t backBufferNum = ppBackbufferTextures->size();

            m_backBufferRTsCreateDataList.resize(backBufferNum);

            for (size_t i = 0; i < backBufferNum; ++i)
            {
                m_backBufferRTsCreateDataList[i].push_back({ static_cast<UINT>(i), ppBackbufferTextures->at(i) });
            }

            parseScene();
        }

        VulkanRenderBase::~VulkanRenderBase()
        {
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
                        Buffer::VulkanVertexBuffer* pVertexBuffer =
                            new Buffer::VulkanVertexBuffer(m_pDevice,
                                                           m_pCmdBufferMgr,
                                                           pModel->GetMesh(j));

                        pVertexBuffer->createVertexBuffer(*m_pHwDevice, TRUE);

                        Buffer::VulkanIndexBuffer* pIndexBuffer =
                            new Buffer::VulkanIndexBuffer(m_pDevice,
                                                          m_pCmdBufferMgr,
                                                          pModel->GetMesh(j));

                        pIndexBuffer->createIndexBuffer(*m_pHwDevice, TRUE);

                        m_mainSceneVertexInputResourceList.push_back(
                        {
                            std::unique_ptr<Buffer::VulkanVertexBuffer>(pVertexBuffer),
                            std::unique_ptr<Buffer::VulkanIndexBuffer>(pIndexBuffer)
                        });
                    }
                }
            }

            const UINT camNum = m_pScene->GetSceneCameraNum();
            const UINT objNum = camNum * modelNum;

            assert(objNum <= DEFAULT_MAX_RENDER_SCENE_OBJ_NUM);

            m_transUniformbuffer.resize(objNum);

            m_descriptorUpdateDataList.push_back({ 0, m_transUniformbuffer.data() });
        }

        VulkanRenderTargetCreateData VulkanRenderBase::genAttachmentCreateData(
            const UINT                             renderSubPassIndex,
            const UINT                             bindingPoint,
            const UINT                             isStore,
            const BX_FRAMEBUFFER_ATTACHMENT_LAYOUT layout,
            const BOOL                             useStencil,
            const BOOL                             isStoreStencil)
        {
            Render::VulkanRenderTargetCreateData rtCreateData = {};
            rtCreateData.renderSubPassIndex = renderSubPassIndex;
            rtCreateData.bindingPoint       = bindingPoint;
            rtCreateData.isStore            = isStore;
            rtCreateData.layout             = layout;
            rtCreateData.useStencil         = useStencil;
            rtCreateData.isStoreStencil     = isStoreStencil;

            return rtCreateData;
        }
    }
}