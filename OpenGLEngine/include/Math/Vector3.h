#pragma once

#include <cstdio>
#include <cstring>

#include <iostream>

#include "Vector2.h"

namespace Math
{
	class Vector3
	{
	public:
		Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f)
			: X(x), Y(y), Z(z)
		{}

		float dot(const Vector3 &v)
		{
			return X * v.X + Y * v.Y + Z * v.Z;
		}

		void crossProduct(const Vector3 &v)
		{
			Vector3 tempVector = *this;
			X = tempVector.Y * v.Z - tempVector.Z * v.Y;
			Y = tempVector.Z * v.X - tempVector.X * v.Z;
			Z = tempVector.X * v.Y - tempVector.Y * v.X;
		}

		~Vector3(){}

		static Vector3 XIdentity() { return Vector3(1, 0, 0); }
		static Vector3 YIdentity() { return Vector3(0, 1, 0); }
		static Vector3 ZIdentity() { return Vector3(0, 0, 1); }

		static float dot(const Vector3 &v1, const Vector3 &v2)
		{
			return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z;
		}

		//TODO: Solve the thread safety problem
		static Vector3& crossProduct(const Vector3 &leftVec3, const Vector3 &rightVec3)
		{
			Vector3 resultVector;
			resultVector.X = leftVec3.Y * rightVec3.Z - leftVec3.Z * rightVec3.Y;
			resultVector.Y = leftVec3.Z * rightVec3.X - leftVec3.X * rightVec3.Z;
			resultVector.Z = leftVec3.X * rightVec3.Y - leftVec3.Y * rightVec3.X;

			return resultVector;
		}
		
		Vector3& operator+(const Vector3 &v)
		{
			return Vector3(X + v.X, Y + v.Y, Z + v.Z);
		}

		Vector3& operator-(const Vector3 &v)
		{
			return Vector3(X - v.X, Y - v.Y, Z - v.Z);
		}

		Vector3& operator*(const Vector3 &v)
		{
			return Vector3(X * v.X, Y * v.Y, Z * v.Z);
		}

		Vector3& operator/(const Vector3 &v)
		{
			return Vector3(X / v.X, Y / v.Y, Z / v.Z);
		}

		void operator+=(float f) { X += f; Y += f; Z += f; }
		void operator-=(float f) { X -= f; Y -= f; Z -= f; }
		void operator*=(float f) { X *= f; Y *= f; Z *= f; }
		void operator/=(float f) { X /= f; Y /= f; Z /= f; }

		void operator+=(const Vector3 &v) { X += v.X; Y += v.Y; Z += v.Z; }
		void operator-=(const Vector3 &v) { X -= v.X; Y -= v.Y; Z -= v.Z; }
		void operator*=(const Vector3 &v) { X *= v.X; Y *= v.Y; Z *= v.Z; }
		void operator/=(const Vector3 &v) { X /= v.X; Y /= v.Y; Z /= v.Z; }

		//float operator*(const Vector3 &v){ return dot(v); }
		void operator=(const Vector3 &v)
		{
			X = v.X;
			Y = v.Y;
			Z = v.Z;
		}

		float X;
		float Y;
		float Z;
	private:
		
	};

	inline std::ostream& operator<<(std::ostream& out, const Vector3 &v)
	{
		return out << v.X << " " << v.Y << " " << v.Z << std::endl;
	}
}
