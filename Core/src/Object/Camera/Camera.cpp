//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include <GLFW/glfw3.h>

#include "Camera.h"
#include "../../Math/Math.h"

namespace Object
{
    namespace Camera
    {
        CameraBase::CameraBase(
            CameraType type,
            const glm::vec3& pos,
            const glm::vec3& center,
            const glm::vec3& up,
            const float      speed,
            const float      nearClip,
            const float      farClip)
            : m_cameraType(type),
            m_trans(pos, center, up),
            curFront(m_trans.GetFront()),
            curRight(m_trans.GetRight()),
            worldUp(up),
            m_nearClip(nearClip),
            m_farClip(farClip)
        {
            this->speed = speed;
        }

        CameraBase::~CameraBase()
        {
        }

        void CameraBase::translate(glm::vec3 translate)
        {
            m_trans.TransPos(translate * speed);
        }

        void CameraBase::rotate(float pitch, float yaw)
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

            //m_trans.front = glm::normalize(glm::vec3(m_front.x, m_front.y, m_front.z));
            //m_trans.right = glm::normalize(glm::vec3(m_right.x, m_right.y, m_right.z));
            //m_trans.up    = glm::normalize(glm::cross(m_trans.right, m_trans.front));

            m_trans.SetTransBase(glm::normalize(glm::vec3(m_front.x, m_front.y, m_front.z)),
                glm::normalize(glm::vec3(m_right.x, m_right.y, m_right.z)));

            curFront = m_trans.GetFront();
            curRight = m_trans.GetRight();
        }

        void CameraBase::update(float deltaTime)
        {
            if (callbackInfo.keyboardCallBack[GLFW_KEY_W] == 1)
            {
                translate(m_trans.GetFront() * deltaTime);
            }
            if (callbackInfo.keyboardCallBack[GLFW_KEY_S] == 1)
            {
                translate(-m_trans.GetFront() * deltaTime);
            }
            if (callbackInfo.keyboardCallBack[GLFW_KEY_A] == 1)
            {
                translate(-m_trans.GetRight() * deltaTime);
            }
            if (callbackInfo.keyboardCallBack[GLFW_KEY_D] == 1)
            {
                translate(m_trans.GetRight() * deltaTime);
            }
            if (callbackInfo.keyboardCallBack[GLFW_KEY_Q] == 1)
            {
                translate(m_trans.GetUp() * deltaTime);
            }
            if (callbackInfo.keyboardCallBack[GLFW_KEY_E] == 1)
            {
                translate(-m_trans.GetUp() * deltaTime);
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

            m_trans.update();
        }

        void CameraBase::setCamTrans(
            const glm::vec3& pos,
            const glm::vec3& center,
            const glm::vec3& up)
        {
            m_trans.SetTrans(pos, center, up);

            curFront = m_trans.GetFront();
            curRight = m_trans.GetRight();
        }
    }
}