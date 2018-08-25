#pragma once

namespace Math
{
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

		float X;
		float Y;
	};
}