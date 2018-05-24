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
	:Camera(CameraType::PROSPECTIVE_CAM, pos, center, up, speed)
{
	this->proj = glm::perspective(fov, aspectRatio, nearClip, farClip);
	this->fov = fov;
	this->nearClip = nearClip;
	this->farClip = farClip;
}

void ProspectiveCamera::translate(glm::vec3 translate)
{
	trans.pos += translate * speed;
	view = trans.getViewMat();
}

void ProspectiveCamera::rotate(float pitch, float yaw)
{
	CLAMP(pitch, -89.0f, 89.0f);

	Vector3 m_front = Vector3::Normalize(
                                Math::rotate(Vector3(curFront),
                                             Vector3(0.0f, 1.0f, 0.0f),
                                             glm::radians(-yaw)));

	m_front = Math::rotate(m_front, Vector3(-m_front.z, 0.0f, m_front.x), glm::radians(pitch));

    Vector3 m_right = Math::rotate(Vector3(curRight),
                                   Vector3(0.0f, 1.0f, 0.0f),
                                   glm::radians(-yaw));
	
	trans.front = glm::normalize(glm::vec3(m_front.x, m_front.y, m_front.z));
	trans.right = glm::normalize(glm::vec3(m_right.x, m_right.y, m_right.z));
	trans.up    = glm::normalize(glm::cross(trans.right, trans.front));

	view = trans.getViewMat();

	curFront = trans.front;
	curRight = trans.right;
}

void ProspectiveCamera::update(float deltaTime)
{
	rotate(static_cast<float>(-callbackInfo.cursorPosCallBack.delta_y) * CAMERA_SENSATIVE, 
		   static_cast<float>( callbackInfo.cursorPosCallBack.delta_x) * CAMERA_SENSATIVE);

	callbackInfo.cursorPosCallBack.reset();
	Camera::update(deltaTime);
}

glm::mat4 ProspectiveCamera::getProjectionMatrix()
{
	return proj;
}
