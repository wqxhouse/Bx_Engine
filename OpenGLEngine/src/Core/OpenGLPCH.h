#pragma once

//General C++ Library
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>

//Include GLEW Library
#define GLEW_STATIC
#include <GL/glew.h>

//Include GLFW Library
#include <GLFW/glfw3.h>

//Include GLM Libarary
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#define IN
#define OUT

#define DEBUG

#define DEFAULT_SHADER_PATH "../Resources/shaders/"

#define CLAMP(v, l, r) \
	((v) = (((v) < (l)) ? (l) : (v))); \
	((v) = (((v) > (r)) ? (r) : (v))); \

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