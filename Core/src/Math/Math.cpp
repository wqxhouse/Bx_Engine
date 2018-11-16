#include <assert.h>

#include "Math.h"
#include "Structures.h"

#define PROJ_DIS 1.0f

#define PROJ_DIS 1.0f

namespace Math
{
    float Radians(
        const float degree)
    {
        float radiance;

        radiance = degree * PI_DIVIDE_ONE_HUNDRED_EIGHTEEN;

        return radiance;
    }

    float Degree(
        const float radian)
    {
        float degree;

        degree = radian * ONE_HUNDRED_EIGHTEEN_DIVIDE_PI;

        return degree;
    }

    Vector3 rotate(
        const Vector3& direction,
        const Vector3& axis,
        const float    angle)
    {
        Quaternion q(axis, angle);
        Vector4 v(direction, 0.0f);
        Quaternion qInv = q.normalQuaternionInverse();

        Quaternion result = q * v * qInv;
        return result.m_v;
    }

    Vector3 rotate(
        const Vector3& direction,
        const Vector3& axis,
        const float    angle1,
        const float    angle2,
        const float    u)
    {
        Quaternion q1(axis, angle1);
        Quaternion q2(axis, angle2);

        Quaternion q = Slerp(q1, q2, u);
        Vector4 v(direction, 0.0f);
        Quaternion qInv = q.normalQuaternionInverse();

        Quaternion result = q * v * qInv;
        return result.m_v;
    }

    Vector3 rotate(
        const Vector3& direction,
        const std::vector<Vector3>& axisList,
        const std::vector<float> angles)
    {
        assert(axisList.size() != angles.size());

        Vector3 res;

        size_t size = axisList.size();

        Quaternion q = Quaternion(axisList[size - 1], angles[size - 1]);

        for (size_t i = size - 2; i >= 0; --i)
        {
            q *= Quaternion(axisList[i], angles[i]);
        }

        Quaternion qInv = q.normalQuaternionInverse();

        Vector4 v(direction, 0.0f);

        Quaternion result = q * v * qInv;

        return result.m_v;
    }

    float Dot(
        const Quaternion& q1,
        const Quaternion& q2)
    {
        float result = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
        return result;
    }

    Quaternion Slerp(
        const Quaternion& q1,
        const Quaternion& q2,
        const float u)
    {
        float cosTheta = Dot(q1, q2);
        float theta = std::acosf(cosTheta);
        float sinThetaInv = 1.0f / std::sqrt(1 - cosTheta * cosTheta);

        Quaternion result = sin((1 - u) * theta) * sinThetaInv * q1 +
            sin(u * theta) * sinThetaInv * q2;

        return result;
    }

    Mat4 viewMatrix(
        const Vector3& eyePos,    ///< Eye(Camera) position
        const Vector3& front,     ///< Front direction
        const Vector3& up)        ///< World up vector
    {
        Mat4 viewMat;

        Vector3 right = Vector3::Normalize(Vector3::crossProduct(front, up));
        Vector3 viewUp = Vector3::crossProduct(right, front);

        Vector3 nFront = Vector3::Normalize(front);
        Vector3 back   = Vector3::Normalize(Vector3(-front.x, -front.y, -front.z));

        viewMat[0][0] = right[0];
        viewMat[1][0] = right[1];
        viewMat[2][0] = right[2];
        viewMat[3][0] = -right.dot(eyePos);

        viewMat[0][1] = viewUp[0];
        viewMat[1][1] = viewUp[1];
        viewMat[2][1] = viewUp[2];
        viewMat[3][1] = -viewUp.dot(eyePos);

        viewMat[0][2] = back[0];
        viewMat[1][2] = back[1];
        viewMat[2][2] = back[2];
        viewMat[3][2] = front.dot(eyePos); // -back.dot(eyePos)

        viewMat[3][3] = 1.0f;

        return viewMat;
    }

    Mat4 prospectiveProjectionMatrix(
        const float fov,             ///< Field of view (Important: Must be in radian)
        const float invAspectRatio,  ///< Inverse aspect ratio (1 / aspectRatio = Height / Width)
        const float nearClip,        ///< Near clip
        const float farClip)         ///< Far clip
    {
        assert(nearClip <= farClip);

        Mat4 projMat;

        float halfFov       = fov * 0.5f;
        float invTangent    = (PROJ_DIS) / std::tan(halfFov);
        float invNagClipDis = 1.0f / (nearClip - farClip);

        projMat[0][0] = invTangent * invAspectRatio;
        projMat[1][1] = invTangent;
        projMat[2][2] = (nearClip + farClip) * invNagClipDis;
        projMat[2][3] = -1;
        projMat[3][2] = 2.0f * nearClip * farClip * invNagClipDis;

        return projMat;
    }

    Mat4 orthographicProjectionMatrix(
        const BxsRectangle& viewport,   ///< Viewport for the orthographic camera
        const float         nearClip,   ///< Near clip
        const float         farClip)    ///< Far clip
    {
        assert(nearClip <= farClip);

        Mat4 projMat;

        float inv_x_dis     = 1.0f / (viewport.right - viewport.left);
        float inv_y_dis     = 1.0f / (viewport.top - viewport.bottom);
        float inv_nag_z_dis = 1.0f / (nearClip - farClip);

        projMat[0][0] = 2.0f * inv_x_dis;
        projMat[1][1] = 2.0f * inv_y_dis;
        projMat[2][2] = 2.0f * inv_nag_z_dis;
        projMat[3][0] = -(viewport.right + viewport.left) * inv_x_dis;
        projMat[3][1] = -(viewport.top + viewport.bottom) * inv_y_dis;
        projMat[3][2] = (nearClip + farClip) * inv_nag_z_dis;
        projMat[3][3] = 1.0f;

        return projMat;
    }

	Quaternion operator*(const float& f, const Quaternion& q)
	{
		Quaternion result = Quaternion(f * q.m_q);
		return result;
	}

	Quaternion operator*(const Vector4& v, const Quaternion& q)
	{
		Quaternion result = Quaternion(v) * q;
		return result;
	}
}