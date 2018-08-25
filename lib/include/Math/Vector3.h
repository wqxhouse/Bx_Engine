#pragma once

#include <cstdio>
#include <cstring>
#include <iostream>
#include <typeinfo>
#include <glm/glm.hpp>

#include "../Memory/MemoryPool.h"

#include "Vector2.h"

namespace Math
{
	class Vector3;
	typedef Vector3* Vector3Ptr;

	class Vector3
	{
	public:
		Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f)
			: X(x), Y(y), Z(z)
		{
			//"Warning: Uncontrolled memory block.(Ignore if you assign memory on stack)
		}

        Vector3(const glm::vec3& v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
        }

		static Vector3Ptr New(float x = 0.0f, float y = 0.0f, float z = 0.0f)
		{
			Vector3Ptr vec3Ptr = new Vector3(x, y, z);
			Memory::MemoryPool::registerMemory<Vector3Ptr>(vec3Ptr);

			return vec3Ptr;
		}

		void setData(float x, float y, float z)
		{
			X = x;
			Y = y;
			Z = z;
		}

		float dot(const Vector3 &v)
		{
			return X * v.X + Y * v.Y + Z * v.Z;
		}

		Vector3 crossProduct(const Vector3 &v)
		{
			Vector3 tempVector = *this;

			X = tempVector.Y * v.Z - tempVector.Z * v.Y;
			Y = tempVector.Z * v.X - tempVector.X * v.Z;
			Z = tempVector.X * v.Y - tempVector.Y * v.X;

            return tempVector;
		}

		~Vector3(){}

		static Vector3 XIdentity() { return Vector3(1, 0, 0); }
		static Vector3 YIdentity() { return Vector3(0, 1, 0); }
		static Vector3 ZIdentity() { return Vector3(0, 0, 1); }

		static float dot(const Vector3 &v1, const Vector3 &v2)
		{
			return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z;
		}

        static Vector3 Normalize(const Vector3& v)
        {
            Vector3 result;

            float lengthInv = 1.0f / (v.x * v.x + v.y * v.y + v.z * v.z);
            result.x = v.x * lengthInv;
            result.y = v.y * lengthInv;
            result.z = v.z * lengthInv;

            return result;
        }

		static Vector3 crossProduct(const Vector3 &leftVec3, const Vector3 &rightVec3)
		{
			Vector3 result;
            result.X = leftVec3.Y * rightVec3.Z - leftVec3.Z * rightVec3.Y;
            result.Y = leftVec3.Z * rightVec3.X - leftVec3.X * rightVec3.Z;
            result.Z = leftVec3.X * rightVec3.Y - leftVec3.Y * rightVec3.X;

			return result;
		}

		static Vector3 crossProduct(const Vector3Ptr leftVec3, const Vector3Ptr rightVec3)
		{
			Vector3 result;
            result.X = leftVec3->Y * rightVec3->Z - leftVec3->Z * rightVec3->Y;
            result.Y = leftVec3->Z * rightVec3->X - leftVec3->X * rightVec3->Z;
            result.Z = leftVec3->X * rightVec3->Y - leftVec3->Y * rightVec3->X;

			return result;
		}
		
		Vector3 operator+(const Vector3 &v)
		{
			return Vector3(X + v.X, Y + v.Y, Z + v.Z);
		}

		Vector3 operator-(const Vector3 &v)
		{
			return Vector3(X - v.X, Y - v.Y, Z - v.Z);
		}

		Vector3 operator*(const Vector3 &v)
		{
			return Vector3(X * v.X, Y * v.Y, Z * v.Z);
		}

		Vector3 operator/(const Vector3 &v)
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
		void operator=(const Vector3& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
		}

        void operator=(const glm::vec3& v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
        }

		Vector3 operator-()
		{
			Vector3 result;
			result.x = -x;
			result.y = -y;
			result.z = -z;

			return result;
		}

		Vector3 operator*(const float f)
		{
			Vector3 result;
			result.X = X * f;
			result.Y = Y * f;
			result.Z = Z * f;

			return result;
		}

		float& operator[](int index)
		{
			switch (index)
			{
			case 0:
				return X;
			case 1:
				return Y;
			case 2:
				return Z;
			default:
				throw std::exception("Index out of range of vector3.(Should be between 0-2)\n");
			}
		}

		union
		{
			struct
			{
				float X;
				float Y;
				float Z;
			};

			struct
			{
				float x;
				float y;
				float z;
			};
		};
	};

	inline Vector3 operator*(const float f, const Vector3& v)
	{
		Vector3 result;
		result.X = f * v.X;
		result.Y = f * v.Y;
		result.Z = f * v.Z;

		return result;
	}

	inline std::ostream& operator<<(std::ostream& out, const Vector3 &v)
	{
		return out << "X:" << v.X << " Y:" << v.Y << " Z:" << v.Z;
	}

	inline std::ostream& operator<<(std::ostream& out, Vector3Ptr vPtr)
	{
		return out << "X:" << vPtr->X << " Y:" << vPtr->Y << " Z:" << vPtr->Z;
	}
}
