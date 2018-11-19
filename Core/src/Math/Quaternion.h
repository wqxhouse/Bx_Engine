//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

/*
    Author: bxs3514
    Date:   05/18/2018
    Reference: http://mathworld.wolfram.com/Quaternion.html
*/

#pragma once

#include "Vector4.h"

namespace Math
{
	class Quaternion
	{
	public:
		Quaternion(const Vector3& v, float angle)
			: m_v(v)
		{
			float halfAngle = angle * 0.5f;
			m_v *= std::sin(halfAngle);
			m_w = std::cos(halfAngle);
		}

		explicit constexpr Quaternion(const Vector4& v)
			: m_q(v) {}

		Quaternion(const float x, const float y, const float z, const float w)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
 		}

		~Quaternion() {};

		float length() const
		{
			return std::sqrt(x * x + y * y + z * z + w * w);
		}

		float dot(const Quaternion& q)
		{
			return m_q.dot(q.m_q);
		}

		Quaternion inverse()
		{
			Vector3 v = -m_v;
			Quaternion res(v.x, v.y, v.z, m_w);
			return res / res.length();
		}

		Quaternion normalQuaternionInverse()
		{
			Vector3 v = -m_v;
			Quaternion res(v.x, v.y, v.z, m_w);
			return res;
		}

		Quaternion operator=(const Quaternion& q)
		{
			x = q.x;
			y = q.y;
			z = q.z;
			w = q.w;
		}

		Quaternion operator+(const Quaternion& q)
		{
			Quaternion result = Quaternion(m_q + q.m_q);
			return result;
		}

		Quaternion operator*(const Quaternion& q)
		{
			Vector4 v;
			v.W    = w * q.w - m_v.dot(q.m_v);
			v.m_v3 = m_w * q.m_v + q.m_w * m_v + Vector3::crossProduct(m_v, q.m_v);

			return Quaternion(v);
		}

		Quaternion operator*(const Vector4& v)
		{
			Quaternion result = (*this) * Quaternion(v);
			return result;
		}

		Quaternion operator/(const float f)
		{
			Quaternion result(m_q / f);
			return result;
		}

		Quaternion operator-()
		{
			Quaternion result(-m_q);
			return result;
		}

		Quaternion operator*=(const Quaternion& q)
		{
			Quaternion res = (*this) * q;
			return res;
		}

		//Quaternion members
		union
		{
			Vector4 m_q;

			struct
			{
				Vector3 m_v;
				float m_w;
			};

			struct
			{
				float x;
				float y;
				float z;
				float w;
			};
		};
	};
}