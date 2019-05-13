//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

namespace Math
{
    INLINE BOOL FloatEqual(const float a, const float b)
    {
        return (abs(a - b) < std::numeric_limits<float>::epsilon());
    }

    INLINE BOOL DoubleEqual(const double a, const double b)
    {
        return (abs(a - b) < std::numeric_limits<double>::epsilon());
    }

	class Vector2
	{
	public:
		Vector2(float x = 0, float y = 0)
			:X(x), Y(y)
		{}

		void setData(float x, float y)
		{
			X = x;
			Y = y;
		}

		~Vector2()
		{}

        union
        {
            struct
            {
                float x;
                float y;
            };

            struct
            {
                float X;
                float Y;
            };
        };
	};
}