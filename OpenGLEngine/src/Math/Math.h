#pragma once

#include "Quaternion.h"
#include "Matrix4x4.h"

#ifndef PI_DIVIDE_ONE_HUNDRED_EIGHTEEN
#define PI_DIVIDE_ONE_HUNDRED_EIGHTEEN 0.017453292f
#endif // PI_DIVIDE_180

#ifndef ONE_HUNDRED_EIGHTEEN_DIVIDE_PI
#define ONE_HUNDRED_EIGHTEEN_DIVIDE_PI 57.29577951f
#endif // 180_DIVIDE_PI

namespace Math
{
    float Radians(const float degree);

    float Degree(const float radiance);

	Vector3 rotate(const Vector3& direction, const Vector3& axis, float angle);

	Vector3 rotate(const Vector3& direction, const Vector3& axis, float angle1, float angle2, float u);

	Vector3 rotate(const Vector3& direction, const std::vector<Vector3>& axisList, const std::vector<float> angles);

	float Dot(const Quaternion& q1, const Quaternion& q2);

	Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, const float u);

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