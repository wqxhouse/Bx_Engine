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
        OrthographicCamera::OrthographicCamera(
            const UINT                          objectId,
            const OrthographicCameraCreateInfo& prosCamCreateInfo)
            : CameraBase(objectId,
                         ORTHOGRAPHIC_CAM,
                         static_cast<CameraCreateInfo>(prosCamCreateInfo)),
              m_viewport(prosCamCreateInfo.viewport)
        {
            m_projectionMatrix = Math::orthographicProjectionMatrix(prosCamCreateInfo.viewport,
                                                                    prosCamCreateInfo.nearClip,
                                                                    prosCamCreateInfo.farClip);
        }

        OrthographicCamera::~OrthographicCamera()
        {
        }

        void OrthographicCamera::update(float deltaTime)
        {
            rotate(static_cast<float>(-callbackInfo.cursorPosCallBack.delta_y) * CAMERA_SENSATIVE,
                   static_cast<float>(callbackInfo.cursorPosCallBack.delta_x)  * CAMERA_SENSATIVE);

            callbackInfo.cursorPosCallBack.reset();
            CameraBase::update(deltaTime);
        }
    }
}