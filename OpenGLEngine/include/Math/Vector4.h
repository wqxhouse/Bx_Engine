#pragma once

#include "Vector3.h"

namespace Math
{
	class Vector4
	{
	public:
		Vector4(float x = 0, float y = 0, float z = 0, float w = 0)
			:X(x), Y(y), Z(z), W(w)
		{}

		float dot(const Vector4 &v) { return X * v.X + Y * v.Y + Z * v.Z + W * v.W; }

		Vector4 crossProduct(const Vector4 &v)
		{
			Vector3 v1 = ToVector3();
			Vector3 v2 = Vector3(v.X / v.W, v.Y / v.W, v.Z / v.W);//TODO: W = 0 ?

			Vector3 res = Vector3::crossProduct(v1, v2);

			X = res.X;
			Y = res.Y;
			Z = res.Z;
			W = 1.0f;
		}

		Vector3& ToVector3()
		{
			//TODO: Use other method to judge 0!
			if (W != 0)
			{
				return Vector3(X / W, Y / W, Z / W);
			}
			else
			{
				return Vector3(X, Y, Z);
			}
		}

		~Vector4()
		{}

		static float dot(const Vector4 &v1, const Vector4 &v2)
		{
			return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z + v1.W * v2.W;
		}

		static Vector4 crossProduct(const Vector4 &v1, const Vector4 &v2)
		{
			//TODO: W = 0 ?
			Vector3 v3_1 = Vector3(v1.X / v1.W, v1.Y / v1.W, v1.Z / v1.W);
			Vector3 v3_2 = Vector3(v2.X / v2.W, v2.Y / v2.W, v2.Z / v2.W);

			Vector3 res = Vector3::crossProduct(v3_1, v3_2);

			return Vector4(res.X, res.Y, res.Z, 1.0f);
		}

		Vector4& operator+(const Vector4 &v)
		{
			return Vector4(X + v.X, Y + v.Y, Z + v.Z, W + v.W);
		}

		Vector4& operator-(const Vector4 &v)
		{
			return Vector4(X - v.X, Y - v.Y, Z - v.Z, W - v.W);
		}

		Vector4& operator*(const Vector4 &v)
		{
			return Vector4(X * v.X, Y * v.Y, Z * v.Z, W * v.W);
		}

		Vector4& operator/(const Vector4 &v)
		{
			return Vector4(X / v.X, Y / v.Y, Z / v.Z, W / v.W);
		}

		void operator+=(float f) { X += f; Y += f; Z += f; W += f; }
		void operator-=(float f) { X -= f; Y -= f; Z -= f; W -= f; }
		void operator*=(float f) { X *= f; Y *= f; Z *= f; W *= f; }
		void operator/=(float f) { X /= f; Y /= f; Z /= f; W /= f; }

		void operator+=(const Vector4 &v) { X += v.X; Y += v.Y; Z += v.Z; W += v.W; }
		void operator-=(const Vector4 &v) { X -= v.X; Y -= v.Y; Z -= v.Z; W -= v.W; }
		void operator*=(const Vector4 &v) { X *= v.X; Y *= v.Y; Z *= v.Z; W *= v.W; }
		void operator/=(const Vector4 &v) { X /= v.X; Y /= v.Y; Z /= v.Z; W /= v.W; }

		void operator=(const Vector4 &v)
		{
			X = v.X;
			Y = v.Y;
			Z = v.Z;
			W = v.W;
		}

		float X;
		float Y;
		float Z;
		float W;
	};

	inline std::ostream& operator<<(std::ostream& out, const Vector4 &v)
	{
		return out << v.X << " " << v.Y << " " << v.Z << " " << v.W << std::endl;
	}
}