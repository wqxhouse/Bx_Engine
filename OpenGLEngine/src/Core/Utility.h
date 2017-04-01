#pragma once

#include <stdio.h>
#include <string.h>

struct CallbackInfo
{
	int keyboardCallBack[1024];
	struct MousePosCallback
	{
		double delta_x;
		double delta_y;
		MousePosCallback() : delta_x(0), delta_y(0) {}
	}cursorPosCallBack;

	CallbackInfo()
		:cursorPosCallBack()
	{
		memset(keyboardCallBack, 0, 1024 * sizeof(int));
	}
};