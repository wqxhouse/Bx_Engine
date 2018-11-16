#pragma once

#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>

#include "../Math/Math.h"

#define LOWER_UPPER_CASE_ASCII_DIFF 32

struct CallbackInfo
{
	int keyboardCallBack[1024];

	struct MousePosCallback
	{
		MousePosCallback() 
            : delta_x(0),
              delta_y(0) 
        {
        }

		inline void reset()
		{
			delta_x = 0.0;
			delta_y = 0.0;
		}

        double delta_x;
        double delta_y;

	}cursorPosCallBack;

	CallbackInfo()
		:cursorPosCallBack()
	{
		 memset(keyboardCallBack, 0, 1024 * sizeof(int));
	}
};

void stringReplace(std::string* pStr, char src, char dst = ' ');

std::vector<std::string> split(const std::string& str, char delim);

std::string ToLowercase(const std::string& str);
std::string ToUppercase(const std::string& str);

// Test
glm::mat4 ToGLMMat4(Math::Mat4 m);