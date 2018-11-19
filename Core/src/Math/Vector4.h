//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "Vector3.h"

namespace Math
{
    class Vector4;
    typedef Vector4* Vector4Ptr;

    class Vector4
    {
    public:
        Vector4()
            : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
        {}

        Vector4(
            const float x,
            const float y,
            const float z,
            const float w)
            : X(x), Y(y), Z(z), W(w)
        {}

        Vector4(const float f)
            : x(f), y(f), z(f), w(f)
        {}

        Vector4(const Vector3& v, float w)
            : m_v3(v), m_w(w)
        {}

        Vector4(float x, const Vector3& v)
        {
            X = x;
            Y = v.x;
            Z = v.y;
            W = v.z;
        }

        Vector4(const Vector3& v)
            : m_v3(v)
        {
            W = 1.0f;
        }

        static Vector4Ptr New(float x = 0, float y = 0, float z = 0, float w = 0)
        {
            Vector4Ptr newVec4Ptr = new Vector4(x, y, z, w);
            return newVec4Ptr;
        }

        void setData(float x, float y, float z, float w)
        {
            X = x;
            Y = y;
            Z = z;
            W = w;
        }

        float dot(const Vector4& v) { return X * v.X + Y * v.Y + Z * v.Z + W * v.W; }

        void crossProduct(const Vector4& v)
        {
            Vector3 v1 = ToVector3();
            Vector3 v2 = v.ToVector3();

            Vector3 res = Vector3::crossProduct(v1, v2);

            X = res.X;
            Y = res.Y;
            Z = res.Z;
            W = 1.0f;
        }

        Vector3 ToVector3() const
        {
            if (w != 0)
            {
                float inv_w = 1.0f / w;
                return Vector3(x * inv_w, y * inv_w, z * inv_w);
            }
            else
            {
                return m_v3;
            }
        }

        ~Vector4()
        {}

        static float dot(const Vector4 &v1, const Vector4 &v2)
        {
            return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
        }

        static Vector4 crossProduct(const Vector4 &v1, const Vector4 &v2)
        {
            //TODO: W = 0 ?
            Vector3 v3_1 = v1.ToVector3();
            Vector3 v3_2 = v2.ToVector3();

            Vector3 res = Vector3::crossProduct(v3_1, v3_2);

            Vector4 newVector4 = Vector4(res, 1.0f);

            return newVector4;
        }

        Vector4 operator+(const Vector4 &v)
        {
            return Vector4(X + v.X, Y + v.Y, Z + v.Z, W + v.W);
        }

        Vector4 operator-(const Vector4 &v)
        {
            return Vector4(X - v.X, Y - v.Y, Z - v.Z, W - v.W);
        }

        Vector4 operator*(const Vector4 &v)
        {
            return Vector4(X * v.X, Y * v.Y, Z * v.Z, W * v.W);
        }

        Vector4 operator/(const Vector4 &v)
        {
            return Vector4(X / v.X, Y / v.Y, Z / v.Z, W / v.W);
        }

        void operator+=(float f) { X += f; Y += f; Z += f; W += f; }
        void operator-=(float f) { X -= f; Y -= f; Z -= f; W -= f; }
        void operator*=(float f) { X *= f; Y *= f; Z *= f; W *= f; }
        void operator/=(float f) 
        {
            float inv_f = 1.0f / f;
            m_v3       *= inv_f;
            w          *= inv_f;
        }

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

        Vector4 operator-()
        {
            Vector4 result;
            result.X = -X;
            result.Y = -Y;
            result.Z = -Z;
            result.W = -W;

            return result;
        }

        float& operator[](int index)
        {
            switch (index)
            {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            case 3:
                return w;
            default:
                throw std::exception("Index out of range of vector4.(Should be between 0-3)\n");
            }
        }
        
        union
        {
            struct
            {
                Vector3 m_v3;
                float m_w;
            };

            struct
            {
                float X;
                float Y;
                float Z;
                float W;
            };

            struct
            {
                float x;
                float y;
                float z;
                float w;
            };

            struct
            {
                float r;
                float g;
                float b;
                float a;
            };
        };
    };
    
    inline Vector4 operator*(const float f, const Vector4& v)
    {
        Vector4 result;
        result.X = f * v.X;
        result.Y = f * v.Y;
        result.Z = f * v.Z;
        result.W = f * v.W;

        return result;
    }

    inline std::ostream& operator<<(std::ostream& out, const Vector4 &v)
    {
        return out << "X:" << v.X << " Y:" << v.Y << " Z:" << v.Z << " W:" << v.W;
    }
    
    inline std::ostream& operator<<(std::ostream& out, Vector4Ptr vPtr)
    {
        return out << "X:" << vPtr->X << " Y:" << vPtr->Y << " Z:" << vPtr->Z << " W:" << vPtr->W;
    }
}