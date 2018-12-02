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
#include "../Object/Camera/Camera.h"
#include "../Object/Light/Light.h"
#include "../Object/Model/Model.h"

namespace Scene
{
    class RenderScene
    {
    public:
        RenderScene(
            const Setting* const pSetting);
        
        RenderScene(
            const Setting* const pSetting,
            const std::string& sceneFile);

        ~RenderScene();

    private:
        const Setting* const m_pSetting;

        std::vector<std::unique_ptr<Object::Camera::CameraBase>> m_pCameraList;
        std::vector<std::unique_ptr<Object::Light::LightBase>>   m_pLightList;
        std::vector<std::unique_ptr<Object::Model::ModelObject>> m_pModelList;
    };
}