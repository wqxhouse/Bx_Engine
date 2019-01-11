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

#define DEFAULT_MAX_RENDER_SCENE_OBJ_NUM 256

namespace Scene
{
    class RenderScene
    {
    public:
        RenderScene(
            const Setting* const     pSetting,
            Memory::MemoryPoolArena* pObjArena,
            const UINT               maxObjNum);

        ~RenderScene();

        // Get the maximum number of objects that can be added to the scene
        INLINE const UINT GetSceneCapacity()
        {
            return m_maxObjNum;
        }

        INLINE void AddProspectiveCamera(
            const Object::Camera::ProspectiveCameraCreateInfo& prosCamCreateInfo)
        {
            m_pCameraList.push_back(
                BX_NEW(Object::Camera::ProspectiveCamera, *m_pObjArena)(m_objNum, prosCamCreateInfo));

            IncrementObjNum();
        }

        INLINE void AddOrthographicCamera(
            const Object::Camera::OrthographicCameraCreateInfo& orthoCamCreateInfo)
        {
            IncrementObjNum();

            m_pCameraList.push_back(
                BX_NEW(Object::Camera::OrthographicCamera, *m_pObjArena)(m_objNum, orthoCamCreateInfo));
        }

        INLINE void AddDirectionalLight(
            const Object::Light::LightCreateInfo& lightCreateInfo)
        {
            IncrementObjNum();

            m_pLightList.push_back(
                BX_NEW(Object::Light::DirectionalLight, *m_pObjArena)(m_objNum, lightCreateInfo));
        }

        INLINE void AddPointLight(
            const Object::Light::PointLightCreateInfo& pointLightCreateInfo)
        {
            IncrementObjNum();

            m_pLightList.push_back(
                BX_NEW(Object::Light::PointLight, *m_pObjArena)(m_objNum, pointLightCreateInfo));
        }

        INLINE void AddSpotLight(
            const Object::Light::SpotLightCreateInfo& spotLightCreateInfo)
        {
            IncrementObjNum();

            m_pLightList.push_back(
                BX_NEW(Object::Light::SpotLight, *m_pObjArena)(m_objNum, spotLightCreateInfo));
        }

        INLINE void AddObjModel(
            const std::string& modelFile,
            const std::string& materialFile,
            Trans*             modelTrans)
        {
            m_pModelList.push_back(
                BX_NEW(Object::Model::ModelObject, *m_pObjArena)(m_objNum, modelFile, materialFile, modelTrans));
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

        static RenderScene CreateRenderScene(
            const Setting* const     pSetting,
            Memory::MemoryPoolArena* pObjArena,
            const std::string&       sceneFile)
        {
            UINT objNum = 0;

            NotImplemented();

            RenderScene renderScene(pSetting, pObjArena, objNum);

            return renderScene;
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
        const UINT                               m_maxObjNum;
    };
}