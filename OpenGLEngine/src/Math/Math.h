#pragma once

#include "Quaternion.h"

namespace Math
{
	Vector3 rotate(const Vector3& direction, const Vector3& axis, float angle);

	Vector3 rotate(const Vector3& direction, const Vector3& axis, float angle1, float angle2, float u);

	Vector3 rotate(const Vector3& direction, const std::vector<Vector3>& axisList, const std::vector<float> angles);

	float Dot(const Quaternion& q1, const Quaternion& q2);

	Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, const float u);

	Quaternion operator*(const float& f, const Quaternion& q);

	Quaternion operator*(const Vector4& v, const Quaternion& q);
}