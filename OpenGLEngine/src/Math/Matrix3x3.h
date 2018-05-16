#pragma once

#include <cstring>
#include "vector3.h"

namespace Math
{
	class Matrix3x3
	{
	public:
		Matrix3x3()
		{
			setMember(0.0f);
		}

		Matrix3x3(float f)
		{
			setMember(f);
		}

		Matrix3x3(float r0, float r1, float r2)
		{
			setMember(r0, r1, r2);
		}

		Matrix3x3(
			float x11, float x12, float x13,
			float x21, float x22, float x23,
			float x31, float x32, float x33)
		{
			setMember(
				x11, x12, x13,
				x21, x22, x23,
				x31, x32, x33);
		}

		void setMember(float f)
		{
            for (size_t i = 0; i < 3; ++i)
            {
                for (size_t j = 0; j < 3; ++j)
                {
                    m[i][j] = f;
                }
            }
		}

		void setMember(float r0, float r1, float r2)
		{
            m[0][0] = r0;
            m[0][1] = r0;
            m[0][2] = r0;

            m[1][0] = r1;
            m[1][1] = r1;
            m[1][2] = r1;

            m[2][0] = r2;
            m[2][1] = r2;
            m[2][2] = r2;
		}

		void setMember(
			float x11, float x12, float x13,
			float x21, float x22, float x23,
			float x31, float x32, float x33)
		{
			m[0][0] = x11;
			m[0][1] = x12;
			m[0][2] = x13;

			m[1][0] = x21;
			m[1][1] = x22;
			m[1][2] = x23;

			m[2][0] = x31;
			m[2][1] = x32;
			m[2][2] = x33;
		}

		float getMember(int x, int y) { return m[x][y]; }

		Vector3Ptr product(const Vector3 &v)
		{
			Vector3Ptr n_v = Vector3::New();

			n_v->X += m[0][0] * v.X + m[0][1] * v.Y + m[0][2] * v.Z;
			n_v->Y += m[1][0] * v.X + m[1][1] * v.Y + m[1][2] * v.Z;
			n_v->Z += m[2][0] * v.X + m[2][1] * v.Y + m[2][2] * v.Z;

			return n_v;
		}

		void product(const Matrix3x3 &m2)
		{
			Matrix3x3 mat = *this;

			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					for (int m = 0, n = 0; m < 3, n < 3; m++, n++)
					{
						this->m[i][j] = mat.m[i][m] * m2.m[n][j];
					}
				}
			}
		}

		void operator=(const Matrix3x3 &m2)
		{
			memcpy(m, &m2, sizeof(m2));
		}

		Vector3Ptr operator[](int i)
		{
			return Vector3::New(m[i][0], m[i][1], m[i][2]);
		}

		Vector3Ptr operator*=(const Vector3 &v)
		{
			return this->product(v);
		}

		void operator*=(const Matrix3x3 &m2)
		{
			this->product(m2);
		}

		~Matrix3x3()
		{}
	private:
		float m[3][3];
	};
}