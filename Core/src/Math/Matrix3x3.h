//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include <cstring>
#include "Vector3.h"
#include "Matrix2x2.h"

namespace Math
{
    template<typename T>
    void swap(T* l, T* r);

	class Matrix3x3
	{
	public:
		Matrix3x3()
		{
			setMember(0.0f);
		}

		Matrix3x3(const float f)
		{
			setMember(f);
		}

		Matrix3x3(const float r0, const float r1, const float r2)
		{
			setMember(r0, r1, r2);
		}

		Matrix3x3(
			const float x11, const float x12, const float x13,
			const float x21, const float x22, const float x23,
			const float x31, const float x32, const float x33)
		{
			setMember(
				x11, x12, x13,
				x21, x22, x23,
				x31, x32, x33);
		}

		void setMember(const float f)
		{
            for (size_t i = 0; i < 3; ++i)
            {
                for (size_t j = 0; j < 3; ++j)
                {
                    m[i][j] = f;
                }
            }
		}

		void setMember(const float r0, const float r1, const float r2)
		{
            m[0][0] = m[0][1] = m[0][2] = r0;
            m[1][0] = m[1][1] = m[1][2] = r1;
            m[2][0] = m[2][1] = m[2][2] = r2;
		}

		void setMember(
			const float x00, const float x01, const float x02,
			const float x10, const float x11, const float x12,
			const float x20, const float x21, const float x22)
		{
			m[0][0] = x00; m[0][1] = x01; m[0][2] = x02;
			m[1][0] = x10; m[1][1] = x11; m[1][2] = x12;
			m[2][0] = x20; m[2][1] = x21; m[2][2] = x22;
		}

		float getMember(const int x, const int y) const { return m[x][y]; }

		Vector3 product(const Vector3 &v) const
		{
			Vector3 n_v;

			n_v.X += m[0][0] * v.X + m[0][1] * v.Y + m[0][2] * v.Z;
			n_v.Y += m[1][0] * v.X + m[1][1] * v.Y + m[1][2] * v.Z;
			n_v.Z += m[2][0] * v.X + m[2][1] * v.Y + m[2][2] * v.Z;

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

        float det() const
        {
            float result = 0.0f;

            result +=  m[0][0] * Mat2::Det(m[1][1], m[1][2], m[2][1], m[2][2]);
            result += -m[0][1] * Mat2::Det(m[1][0], m[1][2], m[2][0], m[2][2]);
            result +=  m[0][2] * Mat2::Det(m[1][0], m[1][1], m[2][0], m[2][1]);

            result += -m[1][0] * Mat2::Det(m[0][1], m[0][2], m[2][1], m[2][2]);
            result +=  m[1][1] * Mat2::Det(m[0][0], m[0][2], m[2][0], m[2][2]);
            result += -m[1][2] * Mat2::Det(m[0][0], m[0][1], m[2][0], m[2][1]);

            result +=  m[2][0] * Mat2::Det(m[0][1], m[0][2], m[1][1], m[1][2]);
            result += -m[2][1] * Mat2::Det(m[0][0], m[0][2], m[1][0], m[2][2]);
            result +=  m[2][2] * Mat2::Det(m[0][0], m[0][1], m[1][0], m[1][1]);

            return result;
        }

        void transpose()
        {
            swap(&m[0][1], &m[1][0]);
            swap(&m[0][2], &m[2][0]);
            swap(&m[1][2], &m[2][1]);
        }

        Matrix3x3 Transpose()
        {
            Matrix3x3 n_m = 
            {
                m[0][0], m[1][0], m[2][0],
                m[0][1], m[1][1], m[2][1],
                m[0][2], m[1][2], m[2][2]
            };

            return n_m;
        }

        void inverse()
        {
            Matrix3x3 mt = Transpose();
            float detValue = det();

            *this =
            {
                Mat2::Det(mt[1][1], mt[1][2], mt[2][1], mt[2][2]),
                Mat2::Det(mt[1][0], mt[1][2], mt[2][0], mt[2][2]),
                Mat2::Det(mt[1][0], mt[1][1], mt[2][0], mt[2][1]),

                Mat2::Det(mt[0][1], mt[0][2], mt[2][1], mt[2][2]),
                Mat2::Det(mt[0][0], mt[0][2], mt[2][0], mt[2][2]),
                Mat2::Det(mt[0][0], mt[0][1], mt[2][0], mt[2][1]),

                Mat2::Det(mt[0][1], mt[0][2], mt[1][1], mt[1][2]),
                Mat2::Det(mt[0][0], mt[0][2], mt[1][0], mt[2][2]),
                Mat2::Det(mt[0][0], mt[0][1], mt[1][0], mt[1][1])
            };

            *this /= detValue;
        }

        Matrix3x3 Inverse()
        {
            Matrix3x3 mt = Transpose();

            Matrix3x3 result =
            {
                Mat2::Det(mt[1][1], mt[1][2], mt[2][1], mt[2][2]),
                Mat2::Det(mt[1][0], mt[1][2], mt[2][0], mt[2][2]),
                Mat2::Det(mt[1][0], mt[1][1], mt[2][0], mt[2][1]),

                Mat2::Det(mt[0][1], mt[0][2], mt[2][1], mt[2][2]),
                Mat2::Det(mt[0][0], mt[0][2], mt[2][0], mt[2][2]),
                Mat2::Det(mt[0][0], mt[0][1], mt[2][0], mt[2][1]),

                Mat2::Det(mt[0][1], mt[0][2], mt[1][1], mt[1][2]),
                Mat2::Det(mt[0][0], mt[0][2], mt[1][0], mt[2][2]),
                Mat2::Det(mt[0][0], mt[0][1], mt[1][0], mt[1][1])
            };

            result /= det();

            return result;
        }

		void operator=(const Matrix3x3 &m2)
		{
			memcpy(m, &m2, sizeof(m2));
		}

		Vector3 operator[](int i)
		{
			return Vector3(m[i][0], m[i][1], m[i][2]);
		}

        void operator+=(const Matrix3x3& m2)
        {
            r0 += m2.r0;
            r1 += m2.r1;
            r2 += m2.r2;
        }

		Vector3 operator*=(const Vector3 &v)
		{
			return this->product(v);
		}

		void operator*=(const Matrix3x3 &m2)
		{
			this->product(m2);
		}

        void operator/=(const float f)
        {
            float invF = 1.0f / f;

            r0 *= f;
            r1 *= f;
            r2 *= f;
        }

		~Matrix3x3()
		{}

	private:
        union
        {
            struct
            {
                Vector3 r0;
                Vector3 r1;
                Vector3 r2;
            };
            float m[3][3];
        };
	};

    inline Matrix3x3 operator*(const Matrix3x3& m1, const Matrix3x3& m2)
    {
        Matrix3x3 result;

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                for (int m = 0, n = 0; m < 3, n < 3; m++, n++)
                {
                    result[i][j] = const_cast<Matrix3x3&>(m1)[i][m] *
                                   const_cast<Matrix3x3&>(m2)[n][j];
                }
            }
        }

        return result;
    }

    typedef Matrix3x3 Mat3;
}