//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../Core/PCH.h"
#include "../Setting/Setting.h"
#include "../Memory/BxMemory.h"
#include "../Object/Camera/Camera.h"
#include "../Object/Light/Light.h"
#include "../Object/Model/Model.h"

namespace Scene
{
    class RenderScene
    {
    public:
        RenderScene(
            const Setting* const     pSetting,
            Memory::MemoryPoolArena* pObjArena);
        
        RenderScene(
            const Setting* const     pSetting,
            Memory::MemoryPoolArena* pObjArena,
            const std::string&       sceneFile);

        ~RenderScene();

        INLINE void addProspectiveCamera(
            const Object::Camera::ProspectiveCameraCreateInfo& prosCamCreateInfo)
        {
            m_pCameraList.push_back(
                BX_NEW(Object::Camera::ProspectiveCamera, *m_pObjArena)(prosCamCreateInfo));
        }

        INLINE void addOrthographicCamera(
            const Object::Camera::OrthographicCameraCreateInfo& orthoCamCreateInfo)
        {
            m_pCameraList.push_back(
                BX_NEW(Object::Camera::OrthographicCamera, *m_pObjArena)(orthoCamCreateInfo));
        }

        INLINE void addDirectionalLight(
            const Object::Light::LightCreateInfo& lightCreateInfo)
        {
            m_pLightList.push_back(
                BX_NEW(Object::Light::DirectionalLight, *m_pObjArena)(lightCreateInfo));
        }

        INLINE void addDirectionalLight(
            const Object::Light::LightCreateInfo& lightCreateInfo)
        {
            m_pLightList.push_back(
                BX_NEW(Object::Light::DirectionalLight, *m_pObjArena)(lightCreateInfo));
        }

        INLINE void addPointLight(
            const Object::Light::PointLightCreateInfo& pointLightCreateInfo)
        {
            m_pLightList.push_back(
                BX_NEW(Object::Light::PointLight, *m_pObjArena)(pointLightCreateInfo));
        }

        INLINE void addDirectionalLight(
            const Object::Light::SpotLightCreateInfo& spotLightCreateInfo)
        {
            m_pLightList.push_back(
                BX_NEW(Object::Light::SpotLight, *m_pObjArena)(spotLightCreateInfo));
        }

        INLINE void addObjModel(
            const std::string& modelFile,
            const std::string& materialFile,
            Trans*             modelTrans)
        {
            m_pModelList.push_back(
                BX_NEW(Object::Model::ModelObject, *m_pObjArena)(modelFile, materialFile, modelTrans));
        }

    private:
        const Setting* const           m_pSetting;

        Memory::MemoryPoolArena* const m_pObjArena;

        std::vector<Object::Camera::CameraBase*> m_pCameraList;
        std::vector<Object::Light::LightBase*>   m_pLightList;
        std::vector<Object::Model::ModelObject*> m_pModelList;
    };
}