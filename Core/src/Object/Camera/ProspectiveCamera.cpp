//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "Camera.h"
#include "../../Math/Math.h"

namespace Object
{
    namespace Camera
    {
        ProspectiveCamera::ProspectiveCamera(
            const ProspectiveCameraCreateInfo& prosCamCreateInfo)
            : CameraBase(CameraType::PROSPECTIVE_CAM,
                         static_cast<CameraCreateInfo>(prosCamCreateInfo))
        {
            m_projectionMatrix = Math::prospectiveProjectionMatrix(
                Math::Radians(fov),
                (1.0f / prosCamCreateInfo.aspectRatio),
                prosCamCreateInfo.nearClip,
                prosCamCreateInfo.farClip);

            this->fov = fov;
        }

        ProspectiveCamera::~ProspectiveCamera()
        {
        }

        void ProspectiveCamera::update(float deltaTime)
        {
            rotate(static_cast<float>(-callbackInfo.cursorPosCallBack.delta_y) * CAMERA_SENSATIVE,
                   static_cast<float>(callbackInfo.cursorPosCallBack.delta_x)  * CAMERA_SENSATIVE);

            callbackInfo.cursorPosCallBack.reset();
            CameraBase::update(deltaTime);
        }
    }
}