#include <assert.h>

#include "Math.h"

namespace Math
{
	Vector3 rotate(const Vector3& direction, const Vector3& axis, float angle)
	{
		Quaternion q(axis, angle);
		Vector4 v(direction, 0.0f);
		Quaternion qInv = q.normalQuaternionInverse();

		Quaternion result = q * v * qInv;
		return result.m_v;
	}

	Vector3 rotate(const Vector3& direction, const Vector3 & axis,
		           float angle1, float angle2, float u)
	{
		Quaternion q1(axis, angle1);
		Quaternion q2(axis, angle2);

		Quaternion q = Slerp(q1, q2, u);
		Vector4 v(direction, 0.0f);
		Quaternion qInv = q.normalQuaternionInverse();

		Quaternion result = q * v * qInv;
		return result.m_v;
	}

	Vector3 rotate(const Vector3& direction, const std::vector<Vector3>& axisList, const std::vector<float> angles)
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