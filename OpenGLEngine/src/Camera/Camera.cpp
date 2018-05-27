#include "Camera.h"
#include "../Math/Math.h"

Camera::Camera(
    CameraType type,
    const glm::vec3& pos,
    const glm::vec3& center,
    const glm::vec3& up,
    const float      speed,
    const float      nearClip,
    const float      farClip)
	: m_cameraType(type),
      m_trans(pos, center, up),
      curFront(glm::normalize(center - pos)),
      worldUp(up),
      m_nearClip(nearClip),
      m_farClip(farClip)
{
	this->speed    = speed;
	this->curRight = m_trans.right;

	m_viewMatrix = glm::lookAt(pos, center, up);
}

Camera::~Camera()
{
}

void Camera::translate(glm::vec3 translate)
{
    m_trans.TransPos(translate * speed);
    m_viewMatrix = m_trans.GetViewMat();
}

void Camera::rotate(float pitch, float yaw)
{
    CLAMP(pitch, -89.0f, 89.0f);

    Math::Vector3 m_front = Math::Vector3::Normalize(
            Math::rotate(Math::Vector3(curFront),
            Math::Vector3(0.0f, 1.0f, 0.0f),
            glm::radians(-yaw)));

    m_front = Math::rotate(
        m_front, Math::Vector3(-m_front.z, 0.0f, m_front.x), glm::radians(pitch));

    Math::Vector3 m_right = Math::rotate(
        Math::Vector3(curRight), Math::Vector3(0.0f, 1.0f, 0.0f), glm::radians(-yaw));

    m_trans.front = glm::normalize(glm::vec3(m_front.x, m_front.y, m_front.z));
    m_trans.right = glm::normalize(glm::vec3(m_right.x, m_right.y, m_right.z));
    m_trans.up    = glm::normalize(glm::cross(m_trans.right, m_trans.front));

    m_viewMatrix = m_trans.GetViewMat();

    curFront = m_trans.front;
    curRight = m_trans.right;
}

void Camera::update(float deltaTime)
{
	if (callbackInfo.keyboardCallBack[GLFW_KEY_W] == 1)
	{
		translate(m_trans.front * deltaTime);
	}
	if (callbackInfo.keyboardCallBack[GLFW_KEY_S] == 1)
	{
		translate(-m_trans.front * deltaTime);
	}
	if (callbackInfo.keyboardCallBack[GLFW_KEY_A] == 1)
	{
		translate(-m_trans.right * deltaTime);
	}
	if (callbackInfo.keyboardCallBack[GLFW_KEY_D] == 1)
	{
		translate(m_trans.right * deltaTime);
	}
	if (callbackInfo.keyboardCallBack[GLFW_KEY_Q] == 1)
	{
		translate(m_trans.up * deltaTime);
	}
	if (callbackInfo.keyboardCallBack[GLFW_KEY_E] == 1)
	{
		translate(-m_trans.up * deltaTime);
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
