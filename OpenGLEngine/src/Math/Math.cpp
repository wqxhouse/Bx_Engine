#include <assert.h>

#include "Math.h"
#include "Structures.h"

#define PROJ_DIS 1.0f

namespace Math
{
    float Radians(const float degree)
    {
        float radiance;

        radiance = degree * PI_DIVIDE_ONE_HUNDRED_EIGHTEEN;

        return radiance;
    }

    float Degree(const float radian)
    {
        float degree;

        degree = radian * ONE_HUNDRED_EIGHTEEN_DIVIDE_PI;

        return degree;
    }

    Vector3 rotate(
        const Vector3& direction,
        const Vector3& axis,
        float          angle)
    {
        Quaternion q(axis, angle);
        Vector4 v(direction, 0.0f);
        Quaternion qInv = q.normalQuaternionInverse();

        Quaternion result = q * v * qInv;
        return result.m_v;
    }

    Vector3 rotate(const Vector3& direction,
        const Vector3& axis,
        float          angle1,
        float          angle2,
        float          u)
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

    float Dot(const Quaternion& q1, const Quaternion& q2)
    {
        float result = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
        return result;
    }

    Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, const float u)
    {
        float cosTheta = Dot(q1, q2);
        float theta = std::acosf(cosTheta);
        float sinThetaInv = 1.0f / std::sqrt(1 - cosTheta * cosTheta);

        Quaternion result = sin((1 - u) * theta) * sinThetaInv * q1 +
            sin(u * theta) * sinThetaInv * q2;

        return result;
    }

    Mat4 viewMatrix(
        Vector3 eyePos,    ///< Eye(Camera) position
        Vector3 center,    ///< The position which the eye look at
        Vector3 up)        ///< World up vector
    {
        Vector3 front = Vector3::Normalize(center - eyePos);
        Vector3 right = Vector3::Normalize(Vector3::crossProduct(front, up));
        Vector3 viewUp = Vector3::crossProduct(right, front);

        Vector3 translate =
        {
            -right.dot(eyePos),
            -viewUp.dot(eyePos),
            -front.dot(eyePos)
        };

        Mat4 viewMat =
        {
            right,     0.0f,
            viewUp,    0.0f,
            front,     0.0f,
            translate, 1.0f
        };

        return viewMat;
    }

    Mat4 prospectiveProjectionMatrix(
        float fov,             ///< Field of view (Important: Must be in radian)
        float invAspectRatio,  ///< Inverse aspect ratio (1 / aspectRatio = Height / Width)
        float nearClip,        ///< Near clip
        float farClip)         ///< Far clip
    {
        assert(nearClip <= farClip);

        Mat4 projMat;

        float halfFov       = fov * 0.5f;
        float invTangent    = (PROJ_DIS) / std::tan(halfFov);
        float invNagClipDis = 1.0f / (nearClip - farClip);

        projMat[0][0] = invTangent * invAspectRatio;
        projMat[1][1] = invTangent;
        projMat[2][2] = (nearClip + farClip) * invNagClipDis;
        projMat[2][3] = 2.0f * nearClip * farClip * invNagClipDis;
        projMat[3][2] = -1;

        return projMat;
    }

    Mat4 orthographicProjectionMatrix(
        const Rectangle viewport,   ///< Viewport for the orthographic camera
        const float     nearClip,   ///< Near clip
        const float     farClip)    ///< Far clip
    {
        assert(nearClip <= farClip);

        Mat4 projMat;

        float inv_x_dis     = 1.0f / (viewport.right - viewport.left);
        float inv_y_dis     = 1.0f / (viewport.top - viewport.bottom);
        float inv_nag_z_dis = 1.0f / (nearClip - farClip);

        projMat[0][0] = 2.0f * inv_x_dis;
        projMat[0][3] = -(viewport.right + viewport.left) * inv_x_dis;
        projMat[1][1] = 2.0f * inv_y_dis;
        projMat[1][3] = -(viewport.top + viewport.bottom) * inv_y_dis;
        projMat[2][2] = 2.0f * inv_nag_z_dis;
        projMat[2][3] = (nearClip + farClip) * inv_nag_z_dis;
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