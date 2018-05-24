#include "Camera.h"

OrthographicCamera::OrthographicCamera(
    const glm::vec3& pos,
    const glm::vec3& center,
    const glm::vec3& up,
    const float speed,
    const float left,
    const float right,
    const float nearClip,
    const float farClip)
    : Camera(ORTHOGRAPHIC_CAM, pos, center, up, speed)
{

}

OrthographicCamera::~OrthographicCamera()
{
}
