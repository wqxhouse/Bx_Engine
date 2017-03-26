#include "Camera.h"

Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, float speed)
	:trans(pos, front, up)
{
	this->speed = speed;
}

glm::mat4 Camera::getViewMat()
{
	return view;
}