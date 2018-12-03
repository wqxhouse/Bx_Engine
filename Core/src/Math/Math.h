//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include <vector>

#include "Quaternion.h"
#include "Matrix4x4.h"
#include "Structures.h"

#ifndef PI_DIVIDE_ONE_HUNDRED_EIGHTEEN
#define PI_DIVIDE_ONE_HUNDRED_EIGHTEEN 0.01745329251994329576923690768489f
#endif // PI_DIVIDE_180

#ifndef ONE_HUNDRED_EIGHTEEN_DIVIDE_PI
#define ONE_HUNDRED_EIGHTEEN_DIVIDE_PI 57.29577951f
#endif // 180_DIVIDE_PI

namespace Math
{
    float Radians(const float degree);

    float Degree(const float radiance);

	Vector3 rotate(
        const Vector3& direction,
        const Vector3& axis,
        const float    angle);

	Vector3 rotate(
        const Vector3& direction,
        const Vector3& axis,
        const float    angle1,
        const float    angle2,
        const float    u);

	Vector3 rotate(
        const Vector3&              direction,
        const std::vector<Vector3>& axisList,
        const std::vector<float>    angles);

    Mat4 Translate(
        const Mat4&    m,
        const Vector3& v);

    Mat4 Scale(
        const Mat4&    m,
        const Vector3& scale);

	float Dot(const Quaternion& q1, const Quaternion& q2);

	Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, const float u);

    Mat4 viewMatrix(
        const Vector3& eyePos,    ///< Eye(Camera) position
        const Vector3& front,     ///< Front direction
        const Vector3& up);       ///< World up vector

    Mat4 prospectiveProjectionMatrix(
        const float fov,             ///< Field of view (Important: Must be in radian)
        const float invAspectRatio,  ///< Inverse aspect ratio (1 / aspectRatio = Height / Width)
        const float nearClip,        ///< Near clip
        const float farClip);        ///< Far clip

    Mat4 orthographicProjectionMatrix(
        const BxsRectangle& viewport,   ///< Viewport for the orthographic camera
        const float         nearClip,   ///< Near clip
        const float         farClip);    ///< Far clip

	Quaternion operator*(const float& f, const Quaternion& q);

	Quaternion operator*(const Vector4& v, const Quaternion& q);

    template<typename T>
    void swap(T* l, T* r)
    {
        T temp = *l;
        *l     = *r;
        *r     = temp;
    }
}