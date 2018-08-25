#pragma once

#include "Vector2.h"

namespace Math
{
    class Matrix2x2
    {
    public:
        Matrix2x2()
        {
            m[0][0] = m[0][1] = 0.0f;
            m[1][0] = m[1][1] = 0.0f;
        }

        Matrix2x2(
            const float r0,
            const float r1)
        {
            m[0][0] = m[0][1] = r0;
            m[1][0] = m[1][1] = r1;
        }

        Matrix2x2(
            const float x00, const float x01,
            const float x10, const float x11)
        {
            m[0][0] = x00; m[0][1] = x01;
            m[1][0] = x10; m[1][1] = x11;
        }

        ~Matrix2x2() {}

        float det() const
        {
            float result = m[0][0] * m[1][1] - m[1][0] * m[0][1];
            return result;
        }

        static float Det(
            const float x00, const float x01,
            const float x10, const float x11)
        {
            float result = x00 * x11 - x10 * x01;
            return result;
        }

    private:
        union
        {
            struct
            {
                Vector2 r0;
                Vector2 r1;
            };
            float m[2][2];
        };
    };

    typedef Matrix2x2 Mat2;
}