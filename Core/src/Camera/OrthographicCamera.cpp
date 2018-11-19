#include "Camera.h"
#include "../Math/Math.h"

namespace Object
{
    namespace Camera
    {
        OrthographicCamera::OrthographicCamera(
            const glm::vec3&    pos,
            const glm::vec3&    center,
            const glm::vec3&    up,
            const float         speed,
            const BxsRectangle& viewport,
            const float         nearClip,
            const float         farClip)
            : CameraBase(ORTHOGRAPHIC_CAM, pos, center, up, speed, nearClip, farClip),
            m_viewport(viewport)
        {
            m_projectionMatrix = Math::orthographicProjectionMatrix(viewport, nearClip, farClip);
        }

        OrthographicCamera::~OrthographicCamera()
        {
        }

        void OrthographicCamera::update(float deltaTime)
        {
            rotate(static_cast<float>(-callbackInfo.cursorPosCallBack.delta_y) * CAMERA_SENSATIVE,
                static_cast<float>(callbackInfo.cursorPosCallBack.delta_x) * CAMERA_SENSATIVE);

            callbackInfo.cursorPosCallBack.reset();
            CameraBase::update(deltaTime);
        }
    }
}