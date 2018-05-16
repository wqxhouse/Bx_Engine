#include "Camera.h"
#include "../Math/Math.h"

using namespace Math;

ProspectiveCamera::ProspectiveCamera(
	glm::vec3 pos, glm::vec3 center, glm::vec3 up, float speed,
	float aspectRatio, float nearClip, float farClip, float fov)
	:Camera(CameraType::PROJECT_CAM, pos, center, up, speed)
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

	glm::mat4 rotationY;
	rotationY = glm::rotate(rotationY, glm::radians(-yaw), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 rotationXFront = glm::normalize(rotationY * glm::vec4(curFront, 1.0f));
	glm::vec3 rotationAxis = glm::vec3(-rotationXFront.z, 0.0f, rotationXFront.x);

	Vector3 m_front = Math::rotate(Vector3(curFront.x, curFront.y, curFront.z), Vector3(0.0f, 1.0f, 0.0f), glm::radians(-yaw));
	m_front = Math::rotate(m_front, Vector3(rotationAxis.x, rotationAxis.y, rotationAxis.z), glm::radians(pitch));
	
	trans.front = glm::normalize(glm::vec3(m_front.x, m_front.y, m_front.z));
	trans.right = glm::normalize(glm::vec3(rotationY * glm::vec4(curRight, 0.0f)));
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
