//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "RenderScene.h"

namespace Scene
{
    RenderScene::RenderScene(
        const Setting* const     pSetting,
        Memory::MemoryPoolArena* pObjArena)
        : m_pSetting(pSetting),
          m_pObjArena(pObjArena)
    {
    }

    RenderScene::RenderScene(
        const Setting* const     pSetting,
        Memory::MemoryPoolArena* pObjArena,
        const std::string&       sceneFile)
        : m_pSetting(pSetting),
          m_pObjArena(pObjArena)
    {
        // TODO: Load scene from XML file
    }

    RenderScene::~RenderScene()
    {
        m_pObjArena->clear();
        m_pCameraList.clear();
        m_pLightList.clear();
        m_pModelList.clear();
    }
}