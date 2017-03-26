#pragma once

#include "../../Core/OpenGLPCH.h"

class Transform
{
public:
	Transform(glm::vec3 pos, glm::vec3 front, glm::vec3 up)
	{
		this->pos = pos;
		this->front = front;
		this->up = up;
	}

	glm::mat4 getViewMat()
	{
		return glm::lookAt(pos, pos + front, up);
	}

	glm::vec3 pos;
	glm::vec3 front;
	glm::vec3 up;
};