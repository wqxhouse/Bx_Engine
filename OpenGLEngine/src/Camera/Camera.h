#pragma once

#include "../Math/Transform/Trans.h"
#include "../Math/Structures.h"

#define CAMERA_SENSATIVE 0.05f
#define HALF_PI          1.57079632f

class Camera
{
public:
    Camera(
        CameraType       type,
        const glm::vec3& pos,
        const glm::vec3& center,
        const glm::vec3& up,
        const float      speed,
        const float      nearClip,
        const float      farClip);

    virtual ~Camera();

    void translate(glm::vec3 trans);
    void rotate(float pitch, float yaw);

    virtual void update(float deltaTime);
    //virtual void draw() {}

    inline CameraType GetCameraType() const { return m_cameraType; }

    inline Trans      GetTrans()            const { return m_trans; }
    inline Math::Mat4 GetViewMatrix()             { return m_trans.GetViewMat(); }
    inline Math::Mat4 GetProjectionMatrix() const { return m_projectionMatrix; }

    void setCamTrans(
        const glm::vec3& pos,
        const glm::vec3& center,
        const glm::vec3& up);

protected:
    Trans m_trans;

    float speed;

    Math::Mat4 m_projectionMatrix;

    glm::vec3 worldUp;

    glm::vec3 curFront;
    glm::vec3 curRight;

private:
    CameraType m_cameraType;

    float m_nearClip;
    float m_farClip;
};

class ProspectiveCamera : public Camera
{
public:
    ProspectiveCamera(
        const glm::vec3& pos,
        const glm::vec3& center,
        const glm::vec3& up,
        const float speed,
        const float invAspectRatio,
        const float nearClip = 0.1f,
        const float farClip  = 100.0f,
        const float fov      = 45.0f);

    void update(float deltaTime);

private:
    float fov;
};

class OrthographicCamera : public Camera
{
public:
    OrthographicCamera(
        const glm::vec3& pos,
        const glm::vec3& center,
        const glm::vec3& up,
        const float      speed,
        const Rectangle& viewport,
        const float      nearClip = 0.1f,
        const float      farClip  = 100.0f);
    ~OrthographicCamera();

    void update(float deltaTime);

private:
    Rectangle m_viewport;
};