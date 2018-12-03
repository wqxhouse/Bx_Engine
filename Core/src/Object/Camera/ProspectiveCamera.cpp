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
            const Math::Vector3& pos,
            const Math::Vector3& center,
            const Math::Vector3& up,
            const float          speed,
            const float          invAspectRatio,
            const float          nearClip,
            const float          farClip,
            const float          fov)
            : CameraBase(CameraType::PROSPECTIVE_CAM, pos, center, up, speed, nearClip, farClip)
        {
            m_projectionMatrix = Math::prospectiveProjectionMatrix(
                Math::Radians(fov), invAspectRatio, nearClip, farClip);

            this->fov = fov;
        }

        ProspectiveCamera::~ProspectiveCamera()
        {
        }

        void ProspectiveCamera::update(float deltaTime)
        {
            rotate(static_cast<float>(-callbackInfo.cursorPosCallBack.delta_y) * CAMERA_SENSATIVE,
                static_cast<float>(callbackInfo.cursorPosCallBack.delta_x) * CAMERA_SENSATIVE);

            callbackInfo.cursorPosCallBack.reset();
            CameraBase::update(deltaTime);
        }
    }
}