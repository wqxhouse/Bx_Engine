#pragma once

//General C++ Library
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>

#define IN
#define OUT

//Include GLEW Library
#define GLEW_STATIC
#include <GL/glew.h>

//Include GLFW Library
#include <GLFW/glfw3.h>

using namespace std;

#define DEFAULT_SHADER_PATH "../Resources/shaders/"

enum AllocateMode
{
	MALLOC, NEW, NEW_ARRAY
};

inline void SafeRelease(void* ptr, AllocateMode allocateMode)
{
	if (ptr != nullptr)
	{
		switch (allocateMode)
		{
		case MALLOC:
			free(ptr);
			break;
		case NEW:
			delete ptr;
		case NEW_ARRAY:
			delete[] ptr;
		}
	}
}