#include "Camera.h"

ProspectiveCamera::ProspectiveCamera(
	glm::vec3 pos, glm::vec3 center, glm::vec3 up, float speed,
	float aspectRatio, float nearClip, float farClip, float fov)
	:Camera(pos, center, up, speed)
{
	this->view = glm::lookAt(pos, center, up);
	this->proj = glm::perspective(fov, aspectRatio, nearClip, farClip);
	this->fov = fov;
	this->nearClip = nearClip;
	this->farClip = farClip;
	this->speed = speed;
}

void ProspectiveCamera::translate(glm::vec3 translate)
{
	trans.pos += translate * speed;
	view = trans.getViewMat();
}

void ProspectiveCamera::rotate(float pitch, float yaw)
{
	//pitch += pitch_offset * CAMERA_SENSATIVE, yaw += yaw_offset * CAMERA_SENSATIVE;
	CLAMP(pitch, -89.0f, 89.0f);

	glm::vec3 direction;
	direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	direction.y = sin(glm::radians(pitch));
	direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	trans.front = glm::normalize(direction);
	trans.right = glm::normalize(glm::cross(trans.front, worldUp));
	//trans.right = glm::vec3(sin(glm::radians(yaw) - HALF_PI), 0, cos(glm::radians(yaw) - HALF_PI));
	trans.up = glm::normalize(glm::cross(trans.right, trans.front));
	view = trans.getViewMat();
}

void ProspectiveCamera::update(float deltaTime)
{
	rotate(-callbackInfo.cursorPosCallBack.delta_y * CAMERA_SENSATIVE, 
		callbackInfo.cursorPosCallBack.delta_x * CAMERA_SENSATIVE);
	Camera::update(deltaTime);
}

glm::mat4 ProspectiveCamera::getProjectionMatrix()
{
	return proj;
}
