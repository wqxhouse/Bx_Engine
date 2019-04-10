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

#include "../Core/TypeDef.h"
#include "Quaternion.h"
#include "Matrix4x4.h"
#include "Shape.h"

namespace Math
{
    INLINE BOOL FloatEqual(const float a, const float b)
    {
        return (abs(a - b) < std::numeric_limits<float>::epsilon());
    }

    INLINE BOOL DoubleEqual(const double a, const double b)
    {
        return (abs(a - b) < std::numeric_limits<double>::epsilon());
    }

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

    Mat4 perspectiveProjectionMatrix(
        const float fov,             ///< Field of view (Important: Must be in radian)
        const float invAspectRatio,  ///< Inverse aspect ratio (1 / aspectRatio = Height / Width)
        const float nearClip,        ///< Near clip
        const float farClip);        ///< Far clip

    Mat4 orthographicProjectionMatrix(
        const Rectangle& viewport,   ///< Viewport for the orthographic camera
        const float      nearClip,   ///< Near clip
        const float      farClip);    ///< Far clip

    INLINE Mat4 operator*(const Mat4& m1, const Mat4& m2)
    {
#ifdef BX_MATRIX_MAJOR_COLUMN
        return Mat4::product(m2, m1);
#else
        return Mat4::product(m1, m2);
#endif // BX_MATRIX_MAJOR
    }

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