#pragma once

#include "Matrix3x3.h"
#include "Vector4.h"

namespace Math
{
	class Matrix4x4
	{
	public:
		Matrix4x4()
		{
			memset(m, 0, sizeof(m));
		}

		Matrix4x4(float f)
		{
			setMember(f);
		}
		
		Matrix4x4(float r0, float r1, float r2, float r3)
		{
			setMember(r0, r1, r2, r3);
		}

		Matrix4x4(
			float x11, float x12, float x13, float x14,
			float x21, float x22, float x23, float x24,
			float x31, float x32, float x33, float x34,
			float x41, float x42, float x43, float x44)
		{
			setMember(
				x11, x12, x13, x14,
				x21, x22, x23, x24,
				x31, x32, x33, x34,
				x41, x42, x43, x44);
		}

		void setMember(float f)
		{
            for (size_t i = 0; i < 4; ++i)
            {
                for (size_t j = 0; j < 4; ++j)
                {
                    m[i][j] = f;
                }
            }
		}

		void setMember(float r0, float r1, float r2, float r3)
		{
            m[0][0] = r0;
            m[0][1] = r0;
            m[0][2] = r0;
            m[0][3] = r0;

            m[1][0] = r1;
            m[1][1] = r1;
            m[1][2] = r1;
            m[1][3] = r1;

            m[2][0] = r2;
            m[2][1] = r2;
            m[2][2] = r2;
            m[2][3] = r2;

            m[3][0] = r3;
            m[3][1] = r3;
            m[3][2] = r3;
            m[3][3] = r3;
		}

		void setMember(
			float x11, float x12, float x13, float x14,
			float x21, float x22, float x23, float x24,
			float x31, float x32, float x33, float x34,
			float x41, float x42, float x43, float x44)
		{
			m[0][0] = x11;
			m[0][1] = x12;
			m[0][2] = x13;
			m[0][3] = x14;

			m[1][0] = x21;
			m[1][1] = x22;
			m[1][2] = x23;
			m[1][3] = x24;

			m[2][0] = x31;
			m[2][1] = x32;
			m[2][2] = x33;
			m[2][3] = x34;

			m[3][0] = x41;
			m[3][1] = x42;
			m[3][2] = x43;
			m[3][3] = x44;
		}

		float getMember(int x, int y) { return m[x][y]; }

		Vector4Ptr product(const Vector4 &v)
		{
			Vector4Ptr n_v = Vector4::New();

			n_v->X += m[0][0] * v.X + m[0][1] * v.Y + m[0][2] * v.Z + m[0][3] * v.W;
			n_v->Y += m[1][0] * v.X + m[1][1] * v.Y + m[1][2] * v.Z + m[1][3] * v.W;
			n_v->Z += m[2][0] * v.X + m[2][1] * v.Y + m[2][2] * v.Z + m[2][3] * v.W;
			n_v->W += m[3][0] * v.X + m[3][1] * v.Y + m[3][2] * v.Z + m[3][3] * v.W;

			return n_v;
		}

		void product(const Matrix4x4 &m2)
		{
			Matrix4x4 mat = *this;

			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					for (int m = 0, n = 0; m < 4, n < 4; m++, n++)
					{
						this->m[i][j] = mat.m[i][m] * m2.m[n][j];
					}
				}
			}
		}

		void operator=(const Matrix4x4 &m2)
		{
			memcpy(m, &m2, sizeof(m2));
		}

		Vector4Ptr operator[](int i)
		{
			return Vector4::New(m[i][0], m[i][1], m[i][2], m[i][3]);
		}

		Vector4Ptr operator*=(const Vector4 &v)
		{
			return this->product(v);
		}

		void operator*=(const Matrix4x4 &m2)
		{
			this->product(m2);
		}

		~Matrix4x4()
		{}

	private:
		float m[4][4];
	};

    typedef Matrix4x4 Mat4;
}