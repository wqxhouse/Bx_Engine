#pragma once

#include "../../Core/OpenGLPCH.h"

class Transform
{
public:
	Transform(glm::vec3 pos, glm::vec3 center, glm::vec3 up)
	{
		this->pos = pos;
		this->front = glm::normalize(center - pos);
		this->up = glm::normalize(up);
		this->right = glm::normalize(glm::cross(front, up));
	}

	glm::mat4 getViewMat()
	{
		return glm::lookAt(pos, pos + front, up);;
	}

	glm::vec3 pos;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
};