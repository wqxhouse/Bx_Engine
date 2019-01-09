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
    const std::vector<const type_info*> RenderScene::m_typeList =
    {
        &typeid(Object::Camera::ProspectiveCamera),
        &typeid(Object::Camera::OrthographicCamera),
        &typeid(Object::Light::DirectionalLight),
        &typeid(Object::Light::PointLight),
        &typeid(Object::Light::SpotLight),
        &typeid(Object::Model::ModelObject)
    };

    RenderScene::RenderScene(
        const Setting* const     pSetting,
        const UINT               maxObjNum,
        Memory::MemoryPoolArena* pObjArena)
        : m_pSetting(pSetting),
          m_pObjArena(pObjArena),
          m_maxObjNum(maxObjNum),
          m_objNum(0)
    {
        //m_typeList.push_back(typeid(Object::Camera::ProspectiveCamera));
    }

    RenderScene::RenderScene(
        const Setting* const     pSetting,
        Memory::MemoryPoolArena* pObjArena,
        const std::string&       sceneFile)
        : m_pSetting(pSetting),
          m_pObjArena(pObjArena)
    {
        // TODO: Load scene from XML file
        NotImplemented();
    }

    RenderScene::~RenderScene()
    {
        m_pObjArena->clear();
        m_pCameraList.clear();
        m_pLightList.clear();
        m_pModelList.clear();
    }
}