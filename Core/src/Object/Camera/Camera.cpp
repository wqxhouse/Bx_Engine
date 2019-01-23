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
            const UINT              objectId,
            const CameraType        type,
            const CameraCreateInfo& camCreateInfo)
            : ObjectBase(objectId, camCreateInfo.pTrans),
              m_cameraType(type),
              m_curFront(m_pTrans->GetFront()),
              m_curRight(m_pTrans->GetRight()),
              m_worldUp(camCreateInfo.pTrans->GetUp()),
              m_nearClip(camCreateInfo.nearClip),
              m_farClip(camCreateInfo.farClip)
        {
            this->speed = speed;
        }

        CameraBase::~CameraBase()
        {
        }

        void CameraBase::translate(
            const Math::Vector3& translate)
        {
            m_pTrans->TransPos(translate * speed);
        }

        void CameraBase::rotate(float pitch, float yaw)
        {            
            CLAMP(pitch, -89.0f, 89.0f);

            Math::Vector3 m_front = Math::Vector3::Normalize(
                Math::rotate(Math::Vector3(m_curFront),
                    Math::Vector3(0.0f, 1.0f, 0.0f),
                    Math::Radians(-yaw)));

            m_front = Math::rotate(
                m_front, Math::Vector3(-m_front.z, 0.0f, m_front.x), Math::Radians(pitch));

            Math::Vector3 m_right = Math::rotate(
                Math::Vector3(m_curRight), Math::Vector3(0.0f, 1.0f, 0.0f), Math::Radians(-yaw));

            m_pTrans->SetTransBase(Math::Vector3::Normalize(m_front),
                                   Math::Vector3::Normalize(Math::Vector3(m_right)));

            m_curFront = m_pTrans->GetFront();
            m_curRight = m_pTrans->GetRight();
        }

        void CameraBase::update(float deltaTime)
        {
            /*if (callbackInfo.keyboardCallBack[GLFW_KEY_W] == 1)
            {
                translate(m_pTrans->GetFront() * deltaTime);
            }
            if (callbackInfo.keyboardCallBack[GLFW_KEY_S] == 1)
            {
                translate(-m_pTrans->GetFront() * deltaTime);
            }
            if (callbackInfo.keyboardCallBack[GLFW_KEY_A] == 1)
            {
                translate(-m_pTrans->GetRight() * deltaTime);
            }
            if (callbackInfo.keyboardCallBack[GLFW_KEY_D] == 1)
            {
                translate(m_pTrans->GetRight() * deltaTime);
            }
            if (callbackInfo.keyboardCallBack[GLFW_KEY_Q] == 1)
            {
                translate(m_pTrans->GetUp() * deltaTime);
            }
            if (callbackInfo.keyboardCallBack[GLFW_KEY_E] == 1)
            {
                translate(-m_pTrans->GetUp() * deltaTime);
            }

            static float originSpeed = speed;
            if (callbackInfo.keyboardCallBack[GLFW_KEY_LEFT_SHIFT] == 1)
            {
                speed = originSpeed * 4;
            }
            else
            {
                speed = originSpeed;
            }*/

            m_pTrans->update();
        }

        void CameraBase::setCamTrans(
            const Math::Vector3& pos,
            const Math::Vector3& center,
            const Math::Vector3& up)
        {
            m_pTrans->SetTrans(pos, center, up);

            m_curFront = m_pTrans->GetFront();
            m_curRight = m_pTrans->GetRight();
        }
    }
}