#include "Camera.h"

ProspectiveCamera::ProspectiveCamera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, float speed)
	:Camera(pos, front, up, speed)
{
	this->view = glm::lookAt(pos, pos + front, up);
	this->speed = speed;
}

void ProspectiveCamera::translate(glm::vec3 translate)
{
	//glm::translate(view, trans);
	glm::normalize(translate);
	trans.pos += translate * speed;
	view = trans.getViewMat();
}

void ProspectiveCamera::rotate(float pitch, float yaw)
{
	glm::vec3 direction;
	direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	direction.y = sin(glm::radians(pitch));
	direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	trans.front = glm::normalize(direction);
	view = trans.getViewMat();
}