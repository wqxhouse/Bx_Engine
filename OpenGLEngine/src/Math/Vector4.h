#pragma once

#include "Vector3.h"

namespace Math
{
	class Vector4;
	typedef Vector4* Vector4Ptr;

	class Vector4
	{
	public:
		Vector4(float x = 0, float y = 0, float z = 0, float w = 0)
			:X(x), Y(y), Z(z), W(w)
		{
			//"Warning: Uncontrolled memory block.(Ignore if you assign memory on stack)
		}

		static Vector4Ptr New(float x = 0, float y = 0, float z = 0, float w = 0)
		{
			Vector4Ptr newVec4Ptr = new Vector4(x, y, z, w);
			Memory::MemoryPool::registerMemory<Vector4Ptr>(newVec4Ptr);

			return newVec4Ptr;
		}

		void setData(float x, float y, float z, float w)
		{
			X = x;
			Y = y;
			Z = z;
			W = w;
		}

		float dot(const Vector4 &v) { return X * v.X + Y * v.Y + Z * v.Z + W * v.W; }

		void crossProduct(Vector4Ptr v)
		{
			Vector3Ptr v1 = ToVector3();
			Vector3Ptr v2 = v->ToVector3();

			Vector3Ptr res = Vector3::crossProduct(v1, v2);

			X = res->X;
			Y = res->Y;
			Z = res->Z;
			W = 1.0f;
		}

		Vector3Ptr ToVector3()
		{
			//TODO: Use other method to judge 0!
			if (W != 0)
			{
				return Vector3::New(X / W, Y / W, Z / W);
			}
			else
			{
				return Vector3::New(X, Y, Z);
			}
		}

		~Vector4()
		{}

		static float dot(const Vector4 &v1, const Vector4 &v2)
		{
			return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z + v1.W * v2.W;
		}

		static Vector4Ptr crossProduct(const Vector4 &v1, const Vector4 &v2)
		{
			//TODO: W = 0 ?
			Vector3 v3_1 = Vector3(v1.X / v1.W, v1.Y / v1.W, v1.Z / v1.W);
			Vector3 v3_2 = Vector3(v2.X / v2.W, v2.Y / v2.W, v2.Z / v2.W);

			Vector3Ptr res = Vector3::crossProduct(v3_1, v3_2);

			Vector4Ptr newVector4Ptr = Vector4::New(res->X, res->Y, res->Z, 1.0f);
			delete res;

			return newVector4Ptr;
		}

		Vector4Ptr operator+(const Vector4 &v)
		{
			return Vector4::New(X + v.X, Y + v.Y, Z + v.Z, W + v.W);
		}

		Vector4Ptr operator-(const Vector4 &v)
		{
			return Vector4::New(X - v.X, Y - v.Y, Z - v.Z, W - v.W);
		}

		Vector4Ptr operator*(const Vector4 &v)
		{
			return Vector4::New(X * v.X, Y * v.Y, Z * v.Z, W * v.W);
		}

		Vector4Ptr operator/(const Vector4 &v)
		{
			return Vector4::New(X / v.X, Y / v.Y, Z / v.Z, W / v.W);
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

		float operator[](int index)
		{
			switch (index)
			{
			case 0:
				return X;
			case 1:
				return Y;
			case 2:
				return Z;
			case 3:
				return W;
			Default:
				throw std::exception("Index out of range of vector4.(Should be between 0-3)\n");
			}
		}

		float X;
		float Y;
		float Z;
		float W;
	};

	inline std::ostream& operator<<(std::ostream& out, const Vector4 &v)
	{
		return out << "X:" << v.X << " Y:" << v.Y << " Z:" << v.Z << " W:" << v.W;
	}
	
	inline std::ostream& operator<<(std::ostream& out, Vector4Ptr vPtr)
	{
		return out << "X:" << vPtr->X << " Y:" << vPtr->Y << " Z:" << vPtr->Z << " W:" << vPtr->W;
	}
}