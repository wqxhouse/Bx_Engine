#pragma once

// General C++ Library
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>

// Include GLEW Library
#define GLEW_STATIC
#include <GL/glew.h>

// Include GLFW Library
#include <GLFW/glfw3.h>

// Include GLM Libarary
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

// Include define types
#include "TypeDef.h"

enum AllocateMode
{
    MALLOC, NEW, NEW_ARRAY
};

inline void SafeRelease(void* ptr, AllocateMode allocateMode)
{
    if (ptr != NULL)
    {
        switch (allocateMode)
        {
        case MALLOC:
            free(ptr);
            break;
        case NEW:
            delete ptr;
            break;
        case NEW_ARRAY:
            delete[] ptr;
            break;
        default:
            break;
        }
    }
}

#include "UtilityGL.h"
extern CallbackInfo callbackInfo;