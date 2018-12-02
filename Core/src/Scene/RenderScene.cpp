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
        const Setting* const pSetting)
        : m_pSetting(pSetting)
    {
    }

    RenderScene::RenderScene(
        const Setting* const pSetting,
        const std::string&   sceneFile)
        : m_pSetting(pSetting)
    {
        // TODO: Load scene from XML file
    }

    RenderScene::~RenderScene()
    {
    }
}