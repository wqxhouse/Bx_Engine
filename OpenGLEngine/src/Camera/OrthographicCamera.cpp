#include "Camera.h"
#include "../Math/Matrix4x4.h"

OrthographicCamera::OrthographicCamera(
    const glm::vec3& pos,
    const glm::vec3& center,
    const glm::vec3& up,
    const float      speed,
    const Rectangle  viewport,
    const float      nearClip,
    const float      farClip)
    : Camera(ORTHOGRAPHIC_CAM, pos, center, up, speed, nearClip, farClip),
      m_viewport(viewport)
{
    m_projectionMatrix = glm::ortho(viewport.left, viewport.right, viewport.bottom, viewport.top);
}

OrthographicCamera::~OrthographicCamera()
{
}

void OrthographicCamera::update(float deltaTime)
{
    rotate(static_cast<float>(-callbackInfo.cursorPosCallBack.delta_y) * CAMERA_SENSATIVE,
           static_cast<float>(callbackInfo.cursorPosCallBack.delta_x) * CAMERA_SENSATIVE);

    callbackInfo.cursorPosCallBack.reset();
    Camera::update(deltaTime);
}
