#pragma once

#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>

struct CallbackInfo
{
	int keyboardCallBack[1024];
	struct MousePosCallback
	{
		double delta_x;
		double delta_y;
		MousePosCallback() : delta_x(0), delta_y(0) {}
		inline void reset()
		{
			delta_x = 0.0;
			delta_y = 0.0;
		}
	}cursorPosCallBack;

	CallbackInfo()
		:cursorPosCallBack()
	{
		memset(keyboardCallBack, 0, 1024 * sizeof(int));
	}
};

void stringReplace(std::string* pStr, char src, char dst = ' ');

std::vector<std::string> split(const std::string& str, char delim);