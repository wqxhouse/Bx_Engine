#include "Camera.h"
#include "../Math/Math.h"

using namespace Math;

ProspectiveCamera::ProspectiveCamera(
    const glm::vec3& pos,
    const glm::vec3& center,
    const glm::vec3& up,
    const float speed,
    const float aspectRatio,
    const float nearClip,
    const float farClip,
    const float fov)
	: Camera(CameraType::PROSPECTIVE_CAM, pos, center, up, speed, nearClip, farClip)
{
	this->m_projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
    Math::Mat4 projectMat = Math::prospectiveProjectionMatrix(Math::Radians(fov), aspectRatio, nearClip, farClip);

	this->fov = fov;
}

void ProspectiveCamera::update(float deltaTime)
{
	rotate(static_cast<float>(-callbackInfo.cursorPosCallBack.delta_y) * CAMERA_SENSATIVE, 
		   static_cast<float>( callbackInfo.cursorPosCallBack.delta_x) * CAMERA_SENSATIVE);

	callbackInfo.cursorPosCallBack.reset();
	Camera::update(deltaTime);
}
