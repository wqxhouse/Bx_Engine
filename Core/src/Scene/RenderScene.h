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
            const UINT               maxObjNum,
            Memory::MemoryPoolArena* pObjArena);
        
        RenderScene(
            const Setting* const     pSetting,
            Memory::MemoryPoolArena* pObjArena,
            const std::string&       sceneFile);

        ~RenderScene();

        INLINE void AddProspectiveCamera(
            const Object::Camera::ProspectiveCameraCreateInfo& prosCamCreateInfo)
        {
            IncrementObjNum();

            m_pCameraList.push_back(
                BX_NEW(Object::Camera::ProspectiveCamera, *m_pObjArena)(prosCamCreateInfo));
        }

        INLINE void AddOrthographicCamera(
            const Object::Camera::OrthographicCameraCreateInfo& orthoCamCreateInfo)
        {
            IncrementObjNum();

            m_pCameraList.push_back(
                BX_NEW(Object::Camera::OrthographicCamera, *m_pObjArena)(orthoCamCreateInfo));
        }

        INLINE void AddDirectionalLight(
            const Object::Light::LightCreateInfo& lightCreateInfo)
        {
            IncrementObjNum();

            m_pLightList.push_back(
                BX_NEW(Object::Light::DirectionalLight, *m_pObjArena)(lightCreateInfo));
        }

        INLINE void AddPointLight(
            const Object::Light::PointLightCreateInfo& pointLightCreateInfo)
        {
            IncrementObjNum();

            m_pLightList.push_back(
                BX_NEW(Object::Light::PointLight, *m_pObjArena)(pointLightCreateInfo));
        }

        INLINE void AddSpotLight(
            const Object::Light::SpotLightCreateInfo& spotLightCreateInfo)
        {
            IncrementObjNum();

            m_pLightList.push_back(
                BX_NEW(Object::Light::SpotLight, *m_pObjArena)(spotLightCreateInfo));
        }

        INLINE void AddObjModel(
            const std::string& modelFile,
            const std::string& materialFile,
            Trans*             modelTrans)
        {
            m_pModelList.push_back(
                BX_NEW(Object::Model::ModelObject, *m_pObjArena)(modelFile, materialFile, modelTrans));
        }

        INLINE Object::Camera::CameraBase* GetCamera(
            const UINT camIndex) const
        {
            return m_pCameraList[camIndex];
        }

        INLINE Object::Light::LightBase* GetLight(
            const UINT lightIndex) const
        {
            return m_pLightList[lightIndex];
        }

        INLINE Object::Model::ModelObject* GetModel(
            const UINT modelIndex) const
        {
            return m_pModelList[modelIndex];
        }

        static INLINE size_t GetMaxObjSize()
        {
            size_t maxSize = 0;

            maxSize = std::max(maxSize, sizeof(Object::Camera::ProspectiveCamera));
            maxSize = std::max(maxSize, sizeof(Object::Camera::OrthographicCamera));
            maxSize = std::max(maxSize, sizeof(Object::Light::DirectionalLight));
            maxSize = std::max(maxSize, sizeof(Object::Light::PointLight));
            maxSize = std::max(maxSize, sizeof(Object::Light::SpotLight));
            maxSize = std::max(maxSize, sizeof(Object::Model::ModelObject));

            return maxSize;
        }

    private:
        INLINE void IncrementObjNum()
        {
            m_objNum++;

            assert(m_objNum <= m_maxObjNum);
        }

        // The all object type supported by scene
        static const std::vector<const type_info*> m_typeList;

        const Setting* const           m_pSetting;

        Memory::MemoryPoolArena* const m_pObjArena;

        std::vector<Object::Camera::CameraBase*> m_pCameraList;
        std::vector<Object::Light::LightBase*>   m_pLightList;
        std::vector<Object::Model::ModelObject*> m_pModelList;

        UINT                                     m_objNum;
        UINT                                     m_maxObjNum;
    };
}