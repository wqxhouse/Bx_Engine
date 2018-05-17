#include "Camera.h"

Camera::Camera(
    CameraType type,
    const glm::vec3& pos,
    const glm::vec3& center,
    const glm::vec3& up,
    const float speed)
	: m_type(type), trans(pos, center, up), curFront(glm::normalize(center - pos)), worldUp(up)
{
	this->speed = speed;
	this->curRight = trans.right;

	view = glm::lookAt(pos, center, up);
	//memset(translationKey, 0, 1024 * sizeof(int));
}

Transform Camera::getTrans()
{
	return trans;
}

void Camera::update(float deltaTime)
{
	if (callbackInfo.keyboardCallBack[GLFW_KEY_W] == 1)
	{
		translate(trans.front * deltaTime);
	}
	if (callbackInfo.keyboardCallBack[GLFW_KEY_S] == 1)
	{
		translate(-trans.front * deltaTime);
	}
	if (callbackInfo.keyboardCallBack[GLFW_KEY_A] == 1)
	{
		translate(-trans.right * deltaTime);
	}
	if (callbackInfo.keyboardCallBack[GLFW_KEY_D] == 1)
	{
		translate(trans.right * deltaTime);
	}
	if (callbackInfo.keyboardCallBack[GLFW_KEY_Q] == 1)
	{
		translate(trans.up * deltaTime);
	}
	if (callbackInfo.keyboardCallBack[GLFW_KEY_E] == 1)
	{
		translate(-trans.up * deltaTime);
	}

	static float originSpeed = speed;
	if (callbackInfo.keyboardCallBack[GLFW_KEY_LEFT_SHIFT] == 1)
	{
		speed = originSpeed * 4;
	}
	else
	{
		speed = originSpeed;
	}
}

glm::mat4 Camera::getViewMatrix()
{
	return view;
}