//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include <assert.h>

#include "Math.h"
#include "Shape.h"

#define BX_COORDINATE_RH
#define BX_CLIP_SPACE_DEPTH_ZERO_TO_ONE

#define PROJ_DIS 1.0f

#define BX_COORDINATE_SYSTEM_LEFT_HAND  0x00000000
#define BX_COORDINATE_SYATEM_RIGHT_HAND 0x00000001

#ifdef BX_COORDINATE_LH
#define BX_COORDINATE_SYSTEM BX_COORDINATE_SYSTEM_LEFT_HAND
#else
#define BX_COORDINATE_SYSTEM BX_COORDINATE_SYATEM_RIGHT_HAND
#endif // BX_COORDINATE_SYSTEM

#define BX_DEPTH_ZERO_TO_ONE         0x00000000
#define BX_DEPTH_NAGATIVE_ONE_TO_ONE 0x00000001

#ifdef BX_CLIP_SPACE_DEPTH_ZERO_TO_ONE
#define BX_CLIP_SPACE_DEPTH BX_DEPTH_ZERO_TO_ONE
#else
#define BX_CLIP_SPACE_DEPTH BX_DEPTH_NAGATIVE_ONE_TO_ONE
#endif // BX_CLIP_SPACE

#ifndef PI_DIVIDE_ONE_HUNDRED_EIGHTEEN
#define PI_DIVIDE_ONE_HUNDRED_EIGHTEEN 0.01745329251994329576923690768489f
#endif // PI_DIVIDE_180

#ifndef ONE_HUNDRED_EIGHTEEN_DIVIDE_PI
#define ONE_HUNDRED_EIGHTEEN_DIVIDE_PI 57.29577951f
#endif // 180_DIVIDE_PI

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

        int size = static_cast<int>(axisList.size());

        Quaternion q = Quaternion(axisList[size - 1], angles[size - 1]);

        for (int i = size - 2; i >= 0; --i)
        {
            q *= Quaternion(axisList[i], angles[i]);
        }

        Quaternion qInv = q.normalQuaternionInverse();

        Vector4 v(direction, 0.0f);

        Quaternion result = q * v * qInv;

        return result.m_v;
    }

    Mat4 Translate(
        const Mat4&    m,
        const Vector3& v)
    {
        Mat4 result(m);

        Vector4 transVec(v, 1.0f);

        result[3] = Vector4(transVec.dot(m[0]),
                            transVec.dot(m[1]),
                            transVec.dot(m[2]),
                            m[3].w);

        return result;
    }

    Mat4 Scale(
        const Mat4&    m,
        const Vector3& scale)
    {
        Mat4 result(m);

        result[0] *= scale.x;
        result[1] *= scale.y;
        result[2] *= scale.z;

        return result;
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

        Vector3 nFront = Vector3::Normalize(front);

        Vector3 right  = Vector3::crossProduct(nFront, up);
        Vector3 viewUp = Vector3::crossProduct(right, nFront);

        Vector3 back   = Vector3(-nFront.x, -nFront.y, -nFront.z);

        viewMat[0][0] = right[0];
        viewMat[1][0] = right[1];
        viewMat[2][0] = right[2];
        viewMat[3][0] = -right.Dot(eyePos);

        viewMat[0][1] = viewUp[0];
        viewMat[1][1] = viewUp[1];
        viewMat[2][1] = viewUp[2];
        viewMat[3][1] = -viewUp.Dot(eyePos);

#if BX_COORDINATE_SYSTEM == BX_COORDINATE_SYATEM_RIGHT_HAND
        viewMat[0][2] = back[0];
        viewMat[1][2] = back[1];
        viewMat[2][2] = back[2];
        viewMat[3][2] = nFront.Dot(eyePos); // -back.dot(eyePos)

#else
        viewMat[0][2] = nFront[0];
        viewMat[1][2] = nFront[1];
        viewMat[2][2] = nFront[2];
        viewMat[3][2] = back.dot(eyePos);  // -nFront.dot(eyePos)
#endif
        viewMat[3][3] = 1.0f;

        return viewMat;
    }

    Mat4 perspectiveProjectionMatrix(
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

#if BX_COORDINATE_SYSTEM == BX_COORDINATE_SYATEM_RIGHT_HAND
        projMat[1][1] = -invTangent;
#else
        projMat[1][1] = invTangent;
#endif

        projMat[2][2] = (nearClip + farClip) * invNagClipDis;
        projMat[2][3] = -1;
        projMat[3][2] = 2.0f * nearClip * farClip * invNagClipDis;

#if BX_CLIP_SPACE_DEPTH == BX_DEPTH_ZERO_TO_ONE
        projMat[2][2] = farClip * invNagClipDis;
        projMat[3][2] = nearClip * farClip * invNagClipDis;
#else
        projMat[2][2] = (nearClip + farClip) * invNagClipDis;
        projMat[3][2] = 2.0f * nearClip * farClip * invNagClipDis;
#endif

        return projMat;
    }

    Mat4 orthographicProjectionMatrix(
        const Rectangle& viewport,   ///< Viewport for the orthographic camera
        const float      nearClip,   ///< Near clip
        const float      farClip)    ///< Far clip
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

    float pointPlaneDis(const Plane& plane, const Vector3& point)
    {
        assert(plane.N.IsNormalized() == TRUE);

        const float d0 = -plane.d;
        const float d1 = point.Dot(plane.N);

        return (d1 - d0);
    }

    Vector3 pointProjOnRay(
        const Vector3& point,
        const Ray&     ray)
    {
        assert(ray.dir.IsNormalized() == TRUE);

        const Vector3 OP = point - ray.origin;

        return ray.dir * OP.Dot(ray.dir);
    }

    Vector3 pointProjOnPlane(
        const Plane&   plane,
        const Vector3& point)
    {
        const float dis = pointPlaneDis(plane, point);

        return (point - (dis * plane.N));
    }

    Vector3 vectorProj(
        const Vector3& v1,
        const Vector3& v2)
    {
        return v2 * (Vector3::dot(v1, v2) / v2.Length());
    }

    Vector3 rayPlaneProj(
        const Plane& plane,
        const Ray&   ray)
    {
        assert(ray.dir.IsNormalized() == TRUE);

        const Vector3 originProj = pointProjOnPlane(plane, ray.origin);
        const Vector3 pointProj  = pointProjOnPlane(plane, ray.origin + ray.dir);

        return (pointProj - originProj);
    }

    BOOL raySphereIntersection(
        const Sphere& sphere,
        const Ray&    ray)
    {
        BOOL intersect = FALSE;

        const Vector3 OC = sphere.center - ray.origin;
        const Vector3 projOC = vectorProj(ray.dir, projOC);

        const Vector3 disVec = projOC - OC;

        intersect = (disVec.Length() <= sphere.radius);

        return intersect;
    }

    BOOL rayAABBIntersection(
        const AABB&    aabb,
        const Vector3& v)
    {
        BOOL intersection = FALSE;

        return intersection;
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