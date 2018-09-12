#include "Camera.h"
#include "../Math/Math.h"

using namespace Math;

ProspectiveCamera::ProspectiveCamera(
    const glm::vec3& pos,
    const glm::vec3& center,
    const glm::vec3& up,
    const float speed,
    const float aspectRatio,
    const float nearClip,
    const float farClip,
    const float fov)
	: Camera(CameraType::PROSPECTIVE_CAM, pos, center, up, speed, nearClip, farClip)
{
	this->m_projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
    // Math::Mat4 projectMat = Math::prospectiveProjectionMatrix(Math::Radians(fov), aspectRatio, nearClip, farClip);

	this->fov = fov;

    float tanHalfFov = std::tan(Math::Radians(fov * 0.5f));
    float nearWidth  = nearClip * tanHalfFov;
    float farWidth   = farClip * tanHalfFov;

    float farOffset = 100.0f;

    m_pFrustumLines[0] = new Line(Vector3(), Vector3(-farWidth - farOffset, -farWidth - farOffset, -farClip - farOffset));
    m_pFrustumLines[1] = new Line(Vector3(), Vector3( farWidth - farOffset, -farWidth - farOffset, -farClip - farOffset));
    m_pFrustumLines[2] = new Line(Vector3(), Vector3( farWidth - farOffset,  farWidth - farOffset, -farClip - farOffset));
    m_pFrustumLines[3] = new Line(Vector3(), Vector3(-farWidth - farOffset,  farWidth - farOffset, -farClip - farOffset));

    m_pFrustumLines[4] = new Line(Vector3(-farWidth - farOffset, -farWidth  - farOffset, -farClip - farOffset), Vector3( farWidth - farOffset, -farWidth - farOffset, -farClip - farOffset));
    m_pFrustumLines[5] = new Line(Vector3( farWidth - farOffset, -farWidth  - farOffset, -farClip - farOffset), Vector3( farWidth - farOffset,  farWidth - farOffset, -farClip - farOffset));
    m_pFrustumLines[6] = new Line(Vector3( farWidth - farOffset,  farWidth  - farOffset, -farClip - farOffset), Vector3(-farWidth - farOffset,  farWidth - farOffset, -farClip - farOffset));
    m_pFrustumLines[7] = new Line(Vector3(-farWidth - farOffset,  farWidth  - farOffset, -farClip - farOffset), Vector3(-farWidth - farOffset, -farWidth - farOffset, -farClip - farOffset));

    m_pFrustumLines[8]  = new Line(Vector3(-nearWidth, -nearWidth, -nearClip), Vector3( nearWidth, -nearWidth, -nearClip));
    m_pFrustumLines[9]  = new Line(Vector3( nearWidth, -nearWidth, -nearClip), Vector3( nearWidth,  nearWidth, -nearClip));
    m_pFrustumLines[10] = new Line(Vector3( nearWidth,  nearWidth, -nearClip), Vector3(-nearWidth,  nearWidth, -nearClip));
    m_pFrustumLines[11] = new Line(Vector3(-nearWidth,  nearWidth, -nearClip), Vector3(-nearWidth, -nearWidth, -nearClip));

    for (Line* pLine : m_pFrustumLines)
    {
        assert(pLine != NULL);

        pLine->initialize();
    }

    glm::vec3 axis = glm::cross(m_trans.GetFront(), glm::vec3(0.0f, 0.0f, 1.0f));
    float cosTheta = glm::dot(m_trans.GetFront(), glm::vec3(0.0f, 0.0f, 1.0f));
    float theta = std::acos(cosTheta);

    m_trans.SetRotation(theta, axis);
}

void ProspectiveCamera::update(float deltaTime)
{
	rotate(static_cast<float>(-callbackInfo.cursorPosCallBack.delta_y) * CAMERA_SENSATIVE, 
		   static_cast<float>( callbackInfo.cursorPosCallBack.delta_x) * CAMERA_SENSATIVE);

	callbackInfo.cursorPosCallBack.reset();
	Camera::update(deltaTime);
}
