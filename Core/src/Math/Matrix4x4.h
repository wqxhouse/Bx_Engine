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

        Matrix4x4
        (
            const Vector3& r0, float f0,
            const Vector3& r1, float f1,
            const Vector3& r2, float f2,
            const Vector3& r3, float f3
        )
        {
            r[0] = Vector4(r0, f0);
            r[1] = Vector4(r1, f1);
            r[2] = Vector4(r2, f2);
            r[3] = Vector4(r3, f3);
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

        Matrix4x4(const glm::mat4& glmMat4)
        {
            setMember(
                glmMat4[0][0], glmMat4[0][1], glmMat4[0][2], glmMat4[0][3],
                glmMat4[1][0], glmMat4[1][1], glmMat4[1][2], glmMat4[1][3],
                glmMat4[2][0], glmMat4[2][1], glmMat4[2][2], glmMat4[2][3],
                glmMat4[3][0], glmMat4[3][1], glmMat4[3][2], glmMat4[3][3]);
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

        Vector4 product(const Vector4 &v)
        {
            Vector4 n_v;

            n_v.x += Vector4::dot(r[0], v);
            n_v.y += Vector4::dot(r[1], v);
            n_v.z += Vector4::dot(r[2], v);
            n_v.w += Vector4::dot(r[3], v);

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
                        this->m[j][i] = mat.m[i][m] * m2.m[n][j];
                    }
                }
            }
        }

        void operator=(const Matrix4x4& m2)
        {
            memcpy(m, &m2, sizeof(m2));
        }

        void operator=(const glm::mat4& m2)
        {
            memcpy(m, &m2, sizeof(m2));
        }

        Vector4& operator[](int i)
        {
            return r[i];
        }

        Vector4 operator*=(const Vector4 &v)
        {
            return this->product(v);
        }

        void operator*=(const Matrix4x4 &m2)
        {
            this->product(m2);
        }

        static Matrix4x4 Identity()
        {
            Matrix4x4 result;
            result.m[0][0] = result.m[1][1] = result.m[2][2] = result.m[3][3] = 1.0f;

            return result;
        }

        static Matrix4x4 product(const Matrix4x4& m1, const Matrix4x4& m2)
        {
            Matrix4x4 result;

            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    for (int m = 0, n = 0; m < 4, n < 4; m++, n++)
                    {
                        result.m[j][i] += m1.m[i][m] * m2.m[n][j];
                    }
                }
            }

            return result;
        }


        ~Matrix4x4()
        {}

    private:
        union
        {
            Vector4 r[4];
            float   m[4][4];
        };
    };

    inline Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2)
    {
        return Matrix4x4::product(m1, m2);
    }

    typedef Matrix4x4 Mat4;
}