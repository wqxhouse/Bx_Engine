#include "Camera.h"

Camera::Camera(glm::vec3 pos, glm::vec3 center, glm::vec3 up, float speed)
	:trans(pos, center, up), delta_x(0.0), delta_y(0.0)
{
	this->speed = speed;
	view = glm::lookAt(pos, center, up);
	memset(translationKey, 0, 1024 * sizeof(int));

	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
}

Transform Camera::getTrans()
{
	return trans;
}

void Camera::updateKeyboardMotion(int key, bool motionValue)
{
	translationKey[key] = motionValue;
}

void Camera::updateMouseMotion(double delta_x, double delta_y)
{
	this->delta_x = static_cast<float>(delta_x);
	this->delta_y = static_cast<float>(delta_y);
}

void Camera::update(float deltaTime)
{
	if (translationKey[GLFW_KEY_W] == 1)
	{
		translate(trans.front * deltaTime);
	}
	if (translationKey[GLFW_KEY_S] == 1)
	{
		translate(-trans.front * deltaTime);
	}
	if (translationKey[GLFW_KEY_A] == 1)
	{
		translate(-trans.right * deltaTime);
	}
	if (translationKey[GLFW_KEY_D] == 1)
	{
		translate(trans.right * deltaTime);
	}
	if (translationKey[GLFW_KEY_Q] == 1)
	{
		translate(trans.up * deltaTime);
	}
	if (translationKey[GLFW_KEY_E] == 1)
	{
		translate(-trans.up * deltaTime);
	}
}

glm::mat4 Camera::getViewMatrix()
{
	return view;
}