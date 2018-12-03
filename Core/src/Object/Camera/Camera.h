//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../../Math/Structures.h"
#include "../ObjectBase.h"

#define CAMERA_SENSATIVE 0.05f
#define HALF_PI          1.57079632f

namespace Object
{
    namespace Camera
    {
        class CameraBase : public ObjectBase
        {
        public:
            CameraBase(
                CameraType           type,
                const Math::Vector3& pos,
                const Math::Vector3& center,
                const Math::Vector3& up,
                const float          speed,
                const float          nearClip,
                const float          farClip);

            virtual ~CameraBase();

            void translate(
                const Math::Vector3& trans);

            void rotate(float pitch, float yaw);

            virtual void update(float deltaTime);
            //virtual void draw() {}

            inline CameraType GetCameraType()       const { return m_cameraType; }

            inline Math::Mat4 GetViewMatrix()             { return m_pTrans->GetViewMat(); }
            inline Math::Mat4 GetProjectionMatrix() const { return m_projectionMatrix; }
            inline float      GetNearClip()         const { return m_nearClip; }
            inline float      GetFarClip()          const { return m_farClip; }

            void setCamTrans(
                const Math::Vector3& pos,
                const Math::Vector3& center,
                const Math::Vector3& up);

        protected:
            float speed;

            Math::Mat4 m_projectionMatrix;

            Math::Vector3 worldUp;

            Math::Vector3 curFront;
            Math::Vector3 curRight;

        private:
            CameraType m_cameraType;

            float m_nearClip;
            float m_farClip;
        };

        class ProspectiveCamera : public CameraBase
        {
        public:
            ProspectiveCamera(
                const Math::Vector3& pos,
                const Math::Vector3& center,
                const Math::Vector3& up,
                const float          speed,
                const float          invAspectRatio,
                const float          nearClip = 0.1f,
                const float          farClip = 100.0f,
                const float          fov = 45.0f);

            ~ProspectiveCamera();

            void update(float deltaTime);

        private:
            float fov;
        };

        class OrthographicCamera : public CameraBase
        {
        public:
            OrthographicCamera(
                const Math::Vector3&    pos,
                const Math::Vector3&    center,
                const Math::Vector3&    up,
                const float             speed,
                const BxsRectangle&     viewport,
                const float             nearClip = 0.1f,
                const float             farClip = 100.0f);

            ~OrthographicCamera();

            void update(float deltaTime);

        private:
            BxsRectangle m_viewport;
        };
    }
}